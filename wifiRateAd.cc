/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************
 * Name : Varun Nair
 * Student Number : 4504550

 **************************************************************************** 
 * For scenario 1: Single User moving away from AP, part of the code is used from the NS3 example rate-adaptation-distance.cc 
  (By Matías Richart <mrichart@fing.edu.uy>)
 *
 * Example usage:
 * To execute a simulation with 4 nodes, Fixed RSS propagation model, MultiUser scenario(Scenario 2) , RTS CTS Enabled and ARF Wifi Manager
 * ./waf --run "scratch/wifiRateAd --nWifi802_11b=4 --rtscts=y --propModel=fixed --scenario=multi --wifiMgr=arf"
 *
 * Possible options for command line arguments are mentioned in Lines 170-176
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/buildings-propagation-loss-model.h"
#include "ns3/internet-module.h"
#include "ns3/enum.h"
#include "ns3/nstime.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>



using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("wifiRateAdaptation");

class NodeStatistics
{
public:
  NodeStatistics (NetDeviceContainer aps, NetDeviceContainer stas);

  void CheckStatistics (double time);

  void RxCallback (std::string path, Ptr<const Packet> packet, const Address &from);
  void SetPosition (Ptr<Node> node, Vector position);
  void AdvancePosition (Ptr<Node> node, int stepsSize, int stepsTime);
  Vector GetPosition (Ptr<Node> node);

  Gnuplot2dDataset GetDatafile ();

private:
  uint32_t m_bytesTotal;
  Gnuplot2dDataset m_output;
};

NodeStatistics::NodeStatistics (NetDeviceContainer aps, NetDeviceContainer stas)
{
  m_bytesTotal = 0;
}

void
NodeStatistics::RxCallback (std::string path, Ptr<const Packet> packet, const Address &from)
{
  m_bytesTotal += packet->GetSize ();
}

void
NodeStatistics::CheckStatistics (double time)
{

}

void
NodeStatistics::SetPosition (Ptr<Node> node, Vector position)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  mobility->SetPosition (position);
}

Vector
NodeStatistics::GetPosition (Ptr<Node> node)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  return mobility->GetPosition ();
}

void
NodeStatistics::AdvancePosition (Ptr<Node> node, int stepsSize, int stepsTime)
{
  Vector pos = GetPosition (node);
  double mbs = ((m_bytesTotal * 8.0) / (1000000.0 * stepsTime));
  mbs = ceilf(mbs*1000)/1000.0;
  mbs = mbs + 0.00;
  m_bytesTotal = 0;
  m_output.Add (pos.x, mbs);
  pos.x += stepsSize;
  SetPosition (node, pos);
  Simulator::Schedule (Seconds (stepsTime), &NodeStatistics::AdvancePosition, this, node, stepsSize, stepsTime);
}

Gnuplot2dDataset
NodeStatistics::GetDatafile ()
{
  return m_output;
}



int 
main (int argc, char *argv[])
{
  double simulationTime = 10.0; //Simulation Time seconds
  double StartTime = 0.0;
  double StopTime = 10.0;
  double throughput = 0.0;
  double throughput_n1 = 0.0;
  double throughput_n2 = 0.0;
  double transmitTime = 0.0;
  double Successk = 2;
  std::string outputFileName = "single_user";
  //uint32_t appRxPackets = 0;
  Time delay = Seconds(0.0);
  std::string manager;
  double rss = -40;   //RSS Vaue for Fixed RSS Loss Model
  uint32_t rtsCtsThresh = 2500;  //RTS CTS Threshold
  //int steps = 10;
  int stepsSize = 2;
  int stepsTime = 1;
  


  // Create randomness based on time
  time_t timex;
  time(&timex);
  RngSeedManager::SetSeed(timex);
  RngSeedManager::SetRun(1);

        
  bool verbose = false;
 

  //Command Line Arguments
  uint32_t nWifi802_11b = 18;     // Number of nodes on 802.11b
  std::string propModel = "fixed";
  std::string wifiMgr = "constant";  
  std::string scenario = "multi"; 
  std::string rtscts = "n"; //Disabled by default

 

  CommandLine cmd;
  cmd.AddValue ("nWifi802_11b", "Total Number of 802.11b STA devices (18 by Default)", nWifi802_11b); 
  cmd.AddValue ("propModel", "Propagation Loss Model To : 'Random' for Random loss model, 'Log' for log distance. By default 'Fixed RSS' is enabled  " propModel);
  cmd.AddValue ("wifiMgr","Type of Rate Adaptation to use: 'constant(default)','aarf','arf'", wifiMgr);
  cmd.AddValue ("rtscts","Enter y (Default) to enable RTSCTS else n",rtscts);
  cmd.AddValue ("scenario","Type of Scenario: single, multi(Default) or mixed",scenario);
  cmd.AddValue ("verbose", "Enable Wifi logging if true", verbose);

  cmd.Parse (argc,argv);        

  NS_LOG_INFO ("Creating Topology");

  //Create STA and AP nodes
  NodeContainer staNodes802_11b;
  staNodes802_11b.Create (nWifi802_11b);
  //std::cout << "Total "<<nWifi802_11b<<" STA Nodes created.." << '\n';


  NodeContainer apNode;  
  apNode.Create (1);     
  NS_LOG_INFO ("AP Node created..");
   


 //Configure the PHY layer model (Default YANS)
  YansWifiChannelHelper channel;

 //Configure the YANS Channel parameters (Loss and Delay)
  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");  

 if (scenario=="single")  //Adjust the simulation time for Single User Scenario
  { 
    simulationTime = 70;
    StopTime = 70;
  }
    
 if (rtscts == "y")    // Enable RTS CTS
  rtsCtsThresh = 200;
 

 if (propModel == "Log")    // Log Distance Propagation Loss
 {
  channel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel","ReferenceLoss",DoubleValue(40.04));
 }

 else                      // Fixed RSS Loss Model
 {
  channel.AddPropagationLoss ("ns3::FixedRssLossModel","Rss",DoubleValue(rss));
 }

  
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.Set ("RxGain", DoubleValue (0) ); 

  phy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 
  
  phy.SetErrorRateModel	("ns3::YansErrorRateModel");    // Error Rate Model(To incorporate channel loss due to poor SNR,when log model is used)

  phy.SetChannel (channel.Create ());

  NS_LOG_INFO ("PHY channel created..");


  //Create the net device containers
  NetDeviceContainer staDevices802_11b;


  //Configure the PHY & MAC Layers for 802.11b and Install the Devices
   WifiHelper wifi;

  wifi.SetStandard(WIFI_PHY_STANDARD_80211b );


  //Configure the RATE CONTROL ALGORITHMS
  if (wifiMgr == "aarf")
    {
     manager = "ns3::AarfWifiManager";
     wifi.SetRemoteStationManager ("ns3::AarfWifiManager","SuccessK",DoubleValue(Successk),"RtsCtsThreshold",UintegerValue(rtsCtsThresh));
    }
  else if (wifiMgr == "arf")
    {   
     manager = "ns3::ArfWifiManager";
     wifi.SetRemoteStationManager ("ns3::ArfWifiManager","RtsCtsThreshold",UintegerValue(rtsCtsThresh));
    }
 else if (wifiMgr == "aarfcd")
    {   
     manager = "ns3::AarfcdWifiManager";
     wifi.SetRemoteStationManager ("ns3::AarfcdWifiManager","SuccessK",DoubleValue(Successk));
    }

  else
    {
     manager = "ns3::ConstantRateWifiManager";
     std::string phyMode ("DsssRate11Mbps");
     wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode",StringValue(phyMode),"ControlMode",StringValue(phyMode));
    }

  if (verbose)                 // If true, enable all logging components of WifiNetDevices
    {
      wifi.EnableLogComponents(); 
    }

  
  
  //Configure a non-QoS upper mac
  NqosWifiMacHelper mac = NqosWifiMacHelper::Default (); 
  Ssid ssid = Ssid ("Hybrid-ssid");
  mac.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid),"ActiveProbing", BooleanValue (false));
  staDevices802_11b = wifi.Install (phy, mac, staNodes802_11b);

  NS_LOG_INFO ("802.11b device configured..");


  //Configure the MAC layer for AP 
  NetDeviceContainer apDevices;
  mac.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid));
  apDevices = wifi.Install (phy, mac, apNode); //Adding 802.11b Net Device to AP



  //Mobility Configuration

  MobilityHelper mobilityAp,mobilitySta;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  
  positionAlloc->Add (Vector (0.0, 0.0, 5));
  mobilityAp.SetPositionAllocator (positionAlloc);
  mobilityAp.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityAp.Install (apNode.Get(0));
  

  //Placing of the Nodes
 if (propModel == "fixed")                     //For Scenario 2
  {
     //Configure the attributes of the RandomBox3dPositionAllocator
     double min = 0.0;
     double max = 5.0;
     double minz = 1;
     double maxz = 2;
     Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
     Ptr<UniformRandomVariable> z = CreateObject<UniformRandomVariable> ();
     x->SetAttribute ("Min", DoubleValue (min));
     x->SetAttribute ("Max", DoubleValue (max));
     z->SetAttribute ("Min", DoubleValue (minz));
     z->SetAttribute ("Max", DoubleValue (maxz));  

     mobilitySta.SetPositionAllocator ("ns3::RandomBoxPositionAllocator",
                                 "X", PointerValue (x),
                                 "Y", PointerValue (x),
                                 "Z", PointerValue (z));

 }

  else                                      // For scenario 1 & 3
 {
   Ptr<ListPositionAllocator> positionAllocSt = CreateObject<ListPositionAllocator> ();
    double xpos = 20;                       // Radius of 20 m
    double ypos;
    
    //Arrange the nodes in a circle 
    for (uint32_t i = 1; i<=nWifi802_11b;i++)
   {
      ypos = sqrt(400 - pow(xpos,2));
      positionAllocSt->Add (Vector (xpos, ypos, 0.5));
      xpos = xpos - 1;  
   }

   mobilitySta.SetPositionAllocator (positionAllocSt); 
  }


      mobilitySta.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  
      mobilitySta.Install (staNodes802_11b);

      if (scenario == "mixed")               // Configure the 1st node to move with Velocity in the + X direction away from AP
    {
      Vector3D pos = Vector3D(0.0,0.0,20);
      Vector3D vel = Vector3D(2.0,0.0,0.0);  // 2 m/s in the + x dir
      mobilitySta.SetMobilityModel ("ns3::ConstantVelocityMobilityModel","Position", Vector3DValue (pos),"Velocity", Vector3DValue (vel));
      mobilitySta.Install (staNodes802_11b.Get(0));
    }
 
      //Statistics counter
      NodeStatistics atpCounter = NodeStatistics (apDevices, staDevices802_11b.Get(0));

      if (scenario == "single") 
    {
     //Move the STA by stepsSize meters every stepsTime seconds
      Simulator::Schedule (Seconds (0.5 + stepsTime), &NodeStatistics::AdvancePosition, &atpCounter, staNodes802_11b.Get (0),stepsSize,     stepsTime);

    }


  //Install the Internet stack in all the Nodes
  InternetStackHelper stack;
  stack.Install (apNode);
  stack.Install (staNodes802_11b);


  //Configure the IPv4 Addresses
  Ipv4AddressHelper address;
  NS_LOG_INFO ("Assign IP Addresses.");

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterfaces, staInterfaces802_11b;
  apInterfaces = address.Assign (apDevices);
  staInterfaces802_11b = address.Assign (staDevices802_11b);


  //Configure Routing
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //Create the OnOff application to send UDP datagrams of size
  // 2000 bytes at a rate of 1.5 Mb/s
  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;   // Discard port (RFC 863)

  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     Address (InetSocketAddress (apInterfaces.GetAddress (0), port)));
  onoff.SetConstantRate (DataRate ("11Mb/s"),1500); //11 Mbps transfer mode with 1500 byte payload
  ApplicationContainer apps = onoff.Install (staNodes802_11b);
  apps.Start (Seconds (StartTime));
  apps.Stop (Seconds (StopTime));


// packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  apps = sink.Install (apNode);
  apps.Start (Seconds (StartTime));
  apps.Stop (Seconds (StopTime));

///////-----------STATS and DATA Collection-------------/////////

  if (scenario == "single") 
    {
      //Register packet receptions to calculate throughput
       Config::Connect ("/NodeList/1/ApplicationList/*/$ns3::PacketSink/Rx",
                   MakeCallback (&NodeStatistics::RxCallback, &atpCounter));
    }
 

  //Set up FlowMon
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();
  


  //Configure Simulation
  Simulator::Stop (Seconds (simulationTime));
  Simulator::Run ();
 
 if (scenario == "single") 
 {
  std::ofstream outfile (("throughput_" + outputFileName + wifiMgr + ".dat").c_str ());
  Gnuplot gnuplot; 
  gnuplot.AddDataset (atpCounter.GetDatafile ());
  gnuplot.GenerateOutput (outfile);
 }

  //Flow Mon
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
 
  for (std::map<FlowId, FlowMonitor::FlowStats>:: const_iterator i = stats.begin(); i!= stats.end(); ++i)
 {
   Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
                
   transmitTime = i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds() ;
   if (transmitTime!= 0)
   throughput = throughput + (i->second.rxBytes * 8.0 / (transmitTime)/1024/1024);
   else
   continue; 

   if (t.sourceAddress == "10.1.1.2")
      throughput_n1 = (i->second.rxBytes * 8.0 / (transmitTime)/1024/1024);
   else if (t.sourceAddress !="10.1.1.1" && t.sourceAddress !="10.1.1.2")
      throughput_n2 = throughput_n2 + (i->second.rxBytes * 8.0 / (transmitTime)/1024/1024);
   else
      throughput = throughput;
   

   if (i->second.rxPackets != 0)
    delay = delay + (i->second.delaySum / i->second.rxPackets);
   else
    continue;
 }
  

  //Application Throughput
  //appRxPackets = server->GetReceived();
  //std::cout<<"Total "<<appRxPackets<<" UDP Packets received by server"<<'\n';
  //std::cout<<" UDP Application Throughput is "<< (appRxPackets * payloadSize * 8)/1024/1024/(StopTime-StartTime-1)<<" Mbps"<<'\n';
  
    std::ofstream avg("avg.dat");  //Output stream for average throughput
    std::ofstream mob("mob.dat");  // Output stream for mobile node throughput
    std::ofstream sta("sta.dat");  // Output stream for stationary node throughput
    if (scenario =="mixed")
   {
    avg << std::fixed;
    avg << std::setprecision(2);
    mob << std::fixed;
    mob << std::setprecision(2);
    sta << std::fixed;
    sta << std::setprecision(2);
    avg << nWifi802_11b <<" "<<throughput<<'\n';    //Aggregate Network Throughput
    mob << nWifi802_11b <<" "<<throughput_n1<<'\n'; //Average Throughput of Mobile Node
    sta << nWifi802_11b <<" "<<throughput_n2/(nWifi802_11b-1)<<'\n'; //Average Node Throughput of Static Nodes
   }
    else if (scenario =="multi")
   {std::cout << std::fixed;
    std::cout << std::setprecision(2);
    std::cout << nWifi802_11b <<" "<<throughput<<'\n';
   }
  
  Simulator::Destroy ();


  return 0;
}
