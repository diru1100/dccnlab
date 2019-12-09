#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/netanim-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/netanim-module.h"
#include "ns3/csma-module.h"
#include <iostream>
#include <tgmath.h>

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("hybrid");

int
main (int argc, char *argv[])
{
// Default number of nodes in the star. Overridable by command line
//argument.
uint32_t nSpokes = 8;
uint32_t nCsma = 4;
LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO); LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

PointToPointHelper pointToPoint;
pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
// star nodes
PointToPointStarHelper star (nSpokes, pointToPoint);

// bus nodes
NodeContainer csmaNodes;
csmaNodes.Create (nCsma);

// container for connecting nodes
NodeContainer p2pNodes;
p2pNodes.Add (csmaNodes.Get (0));
p2pNodes.Add (star.GetHub());

// internet stack
InternetStackHelper stack;
star.InstallStack (stack);
stack.Install (csmaNodes);

NetDeviceContainer p2pDevices;
p2pDevices = pointToPoint.Install (p2pNodes);

CsmaHelper csma;
csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

NetDeviceContainer csmaDevices;
csmaDevices = csma.Install (csmaNodes);

Ipv4AddressHelper address1;
address1.SetBase ("10.1.10.0", "255.255.255.0");
star.AssignIpv4Addresses (address1);

Ipv4AddressHelper address2;
address2.SetBase ("10.2.11.0", "255.255.255.0");
address2.Assign (p2pDevices);

Ipv4AddressHelper address3;
address3.SetBase ("10.3.12.0", "255.255.255.0");
address3.Assign (csmaDevices);

// set positions on graph
AnimationInterface anim("hbs.xml");
// set hub in center
anim.SetConstantPosition(star.GetHub (), 29, 29);
anim.SetConstantPosition(csmaNodes.Get (0), 70, 60);

// set nodes in a circle around the hub
const float angle = (M_PI / 180) * (360/ star.SpokeCount ());
for (uint32_t i = 0; i < star.SpokeCount (); ++i) {
float x = (20 * cos(i * angle)) + 29;
float y = (20 * sin(i * angle)) + 29;
anim.SetConstantPosition(star.GetSpokeNode (i), x, y);
}
for (uint32_t i = 0; i < nCsma; ++i) {
float x = (20 * cos(i * angle)) + 70;
float y = (20 * sin(i * angle)) + 60;
anim.SetConstantPosition(csmaNodes.Get (i), x, y);
}

// install server on any star node
UdpEchoServerHelper echoHelper (9);
ApplicationContainer serverApps = echoHelper.Install(star.GetSpokeNode (6));
serverApps.Start (Seconds (1.0));
serverApps.Stop (Seconds (10.0));

// first client is a star node
ApplicationContainer clientApps1;
UdpEchoClientHelper echoClient1 (star.GetSpokeIpv4Address (6), 9);
echoClient1.SetAttribute ("MaxPackets", UintegerValue (1));
echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));
clientApps1.Add (echoClient1.Install (star.GetSpokeNode (1)));
clientApps1.Start (Seconds (1.0));
clientApps1.Stop (Seconds (6.0));

// second client is a csma node
ApplicationContainer clientApps2;
UdpEchoClientHelper echoClient2 (star.GetSpokeIpv4Address (6), 9);
echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));
clientApps2.Add (echoClient2.Install (csmaNodes.Get (2)));
clientApps2.Start (Seconds (3.0));
clientApps2.Stop (Seconds (10.0));

// Turn on global static routing so we can actually be routed across
//the networks
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
// Turn on pcap tracing
pointToPoint.EnablePcapAll ("hbs");
Simulator::Run ();
Simulator::Destroy ();
return 0;
}