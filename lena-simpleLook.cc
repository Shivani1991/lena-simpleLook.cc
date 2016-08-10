
/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
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
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include <ns3/buildings-helper.h>
//#include "ns3/gtk-config-store.h"

using namespace ns3;

int main (int argc, char *argv[])
{	
  CommandLine cmd;
  cmd.Parse (argc, argv);
	
  // to save a template default attribute file run it like this:
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Save --          
  // ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim
  
  // to load a previously created default attribute file
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Load --   
  // ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();
  
  // Value of ns-3 can be stored in ASCII or XML format:
  // Attributes of Configure Store are "Mode","Filename","Fileformat"
  // There are two mode "load" and "store" : "load" mode load the previously store value and "save" mode 
  // save the load value.
  // Fileformat can be ASCII(Rawtext) or XML.



  // Parse again so you can override default values from the command line
  cmd.Parse (argc, argv);

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  // There are two types of LTEHelper available Building and Mobility helper.
  // MobilityHelper is used to movement of nodes, Most of the nodes in wireless are Mobile in nature.
  // BuildingHelper is used for for install mobility nodes and make Mobility model consistent. 
  

  // Uncomment to enable logging
  // lteHelper->EnableLogComponents ();

  // Create Nodes: One eNodeB(Evolved Base Node) and Two UE(User Equipment Node).
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (2);

  // Install Mobility Model
  // Mobility Model is configure the building information of nodes like bool indoor(), setOutdoor() methods.
  // In this Nodes positions are constants, Only data is transfer between enodeB and UE nodes. 
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (enbNodes);
  BuildingsHelper::Install (enbNodes);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (ueNodes);
  BuildingsHelper::Install (ueNodes);
  

  // Create Devices Containers and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  // Default scheduler is PF(Propational Fair), uncomment to use RR(Round Robin).
  // lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");

  enbDevs = lteHelper->InstallEnbDevice (enbNodes);
  ueDevs = lteHelper->InstallUeDevice (ueNodes);

  // Attach a UE to a eNB for provide the radio link between both of them.
  lteHelper->Attach (ueDevs, enbDevs.Get (0));

  // Activate a data radio bearer
  // Qci is Quality of Service(QoS) Class Indentifier Which having nine different indentifier and each Qci have different priority.
  // GBR_CONV_VOICE is Qci which give Guarnteed Bit Rate for Voice data.
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  lteHelper->ActivateDataRadioBearer (ueDevs, bearer);
  lteHelper->EnableTraces ();

  // There are Mac, pdcp, Rlc and Sinrstats  trace file available in lte.

  Simulator::Stop (Seconds (5));

  Simulator::Run ();

  // GtkConfigStore config;
  // config.ConfigureAttributes ();

  Simulator::Destroy ();
  return 0;
}
