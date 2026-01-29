/*
 * BVSChannel.h
 * Copyright (c) 2024 Technische Universität Berlin 
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  Created on: 2023. 12. 6.
 *      Author: Laurenz Ebner
 */

#ifndef CLASS_BVSChannel_
#define CLASS_BVSChannel_

#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include "terahertz.h"


// parameter for channel model (terrahertz)
#define TESTPACKETSIZE 200
#define FREQ_THZ 0.5*pow(10,12)
#define DIST_INIT 850*pow(10,-6)
#define SKIN_THICKNESS 76*pow(10,-6)
#define VESSEL_THICKNESS 200*pow(10,-6)
#define TISSUETHICKNESS 1*pow(10,-3)
#define POWER 4*pow(10,3)


using namespace std;

namespace ns3{


class BVSChannel;
class GatewayNetDevice;
class NanoNetDevice;

// Define structure of data transmission package 
struct MAC_PHY_DATA {

        int nanobot_ID; // ID of nanobot
        int tissue_ID; // ID of body part where to measure         
        vector<int> PDU_TX; // Bit stream of measurement data at nanobot
        vector<double> SEQ_TX; // Amplitude stream of measurement data send at nanobot
        vector<double> SEQ_RX; // Amplitude stream of measurement data received at gateway
        vector<int> PDU_RX; // decoded bit stream of measurement data at gateway
        
};

/**
* \brief BVSChannel models the communication channel between nanobots and gateway devices 
*
* It contains a list of pointers to all gateway devices and when the send() function is called finds the right gateway device
* according to the position ID inside the human body.
*/
class BVSChannel : public Channel
{

public :
	//static TypeId GetTypeId(void);
	
	//add a general netdevice pointer to the channel
	void Add(Ptr<NetDevice> device); 

	//add a gateway netdevice pointer to the channel
	void AddGateway(Ptr<GatewayNetDevice> device);

	//transmit function where the channel calculations are made
	void send(Ptr<NanoNetDevice> ndev, int pos);

	//find the corresponding gateway netdevice at the current ID
	Ptr<GatewayNetDevice> findGateway( int gatewayID);

	//constructor
	BVSChannel(float vesselthickness);

	//deconstructor
	~BVSChannel();

	//create noise sequence according to the parameters 
	vector<double> createNoiseSequence();

	// inherited from ns3::Channel
  	size_t GetNDevices (void) const override;
  	Ptr<NetDevice> GetDevice (size_t i) const override;

private :
	vector<Ptr<NetDevice> > m_devices;
	vector<Ptr<GatewayNetDevice> > m_gatedevices;

	float m_vesselthickness;
};

};

#endif
