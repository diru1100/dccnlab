#include "ns3/core-module.h"
#include "ns3/network-module.h" #include "ns3/internet-module.h" #include "ns3/point-to-point-module.h" #include "ns3/applications-module.h" #include "ns3/netanim-module.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");
int
main (int argc, char *argv[]) {
CommandLine cmd; cmd.Parse (argc, argv);
Time::SetResolution (Time::NS);
LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO); LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
NodeContainer nodes; nodes.Create(3);
PointToPointHelper p2p1; PointToPointHelper p2p2;
p2p1.SetDeviceAttribute ("DataRate",StringValue ("5Mbps"));

p2p2.SetDeviceAttribute ("DataRate",StringValue ("5Mbps")); p2p1.SetChannelAttribute ("Delay",StringValue ("2ms")); p2p2.SetChannelAttribute ("Delay",StringValue ("2ms"));
NetDeviceContainer device1,device2;
device1 = p2p1.Install(nodes.Get(0),nodes.Get(1)); device2 = p2p2.Install(nodes.Get(1),nodes.Get(2));
InternetStackHelper stack; stack.Install (nodes);
Ipv4AddressHelper h1,h2;
h1.SetBase ("10.1.1.0", "255.255.255.0"); Ipv4InterfaceContainer inter1 = h1.Assign (device1);
h2.SetBase("10.1.2.0","255.255.255.0"); Ipv4InterfaceContainer inter2 = h2.Assign (device2);
UdpEchoServerHelper echoServer(9);
ApplicationContainer serverApps = echoServer.Install (nodes.Get(1)); serverApps.Start (Seconds (1.0));
serverApps.Stop (Seconds (10.0));
UdpEchoClientHelper echoClient1 (inter1.GetAddress(1),9); echoClient1.SetAttribute("MaxPackets",UintegerValue(1)); echoClient1.SetAttribute("Interval",TimeValue(Seconds(1.0))); echoClient1.SetAttribute("PacketSize",UintegerValue(1024));
ApplicationContainer clientApps1 = echoClient1.Install(nodes.Get(0)); clientApps1.Start(Seconds(2.0));
clientApps1.Stop(Seconds(10.0));
UdpEchoClientHelper echoClient (inter2.GetAddress (0), 9); echoClient.SetAttribute ("MaxPackets", UintegerValue (1)); echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0))); echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
ApplicationContainer clientApps2 = echoClient.Install (nodes.Get (2)); clientApps2.Start (Seconds (2.0));
clientApps2.Stop (Seconds (10.0));
AnimationInterface anim("three_nodes.xml");
Simulator::Run ();
Simulator::Destroy (); return 0;
}
