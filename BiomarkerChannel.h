/*
 * BiomarkerChannel.h
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
 *  Created on: 2024. 04. 15.
 *      Author: Saswati Pal
 */

#ifndef CLASS_BIOMARKERCHANNEL_
#define CLASS_BIOMARKERCHANNEL_

#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include <iostream>
#include <vector>

// Include headers 
#include "BiomarkerNetDevice.h"
#include "NanoNetDevice.h" 
#include "Biomarker.h"

namespace ns3 {

class BiomarkerChannel;
class NanoNetDevice; 
class BiomarkerNetDevice;
class Biomarker; //forward declaration
class Nanobot;
class Stream;
class Bloodvessel;

/**
 * \brief BiomarkerChannel models the communication channel between biomarkers and nanobots.
 *
 * It contains a list of pointers to all biomarker devices and when the send() function is called it find the right nanobot device
 * according to the position ID inside the human body.
 */
class BiomarkerChannel : public Channel {
public:
    BiomarkerChannel();
    ~BiomarkerChannel();

    // Add a BiomarkerNetDevice pointer to the channel
    void AddBiomarkerDevice(Ptr<BiomarkerNetDevice> device);

    // The core method for transmitting biomarker data ///void SendToNanobots(Ptr<Packet> packet, Ptr<Biomarker> biomarker, Ptr<Nanobot> nanobot);
    void Send(Ptr<BiomarkerNetDevice> bdev); 

    // inherited from ns3::Channel | Overrides from the Channel base class
    size_t GetNDevices(void) const override;
    Ptr<NetDevice> GetDevice(size_t i) const override;

    /**
     * \brief Broadcasts biomarker data to all nanobots within range of the given biomarker.
     *
     * \param packet The packet containing the biomarker data.
     * \param biomarker A pointer to the Biomarker object.
     */
    void SendToNanobots(Ptr<Packet> packet, Ptr<Biomarker> biomarker);


    // The crucial method for finding a nearby nanobot
    ///Ptr<NanoNetDevice> FindNanobot(Ptr<BiomarkerNetDevice> bdev); 

private:
    // Store pointers to the BiomarkerNetDevice objects in the channel
    std::vector<Ptr<BiomarkerNetDevice>> m_biomarkerDevices;

    // Potentially other member variables related to channel characteristics,
    // propagation models, etc. 
};

} // namespace ns3

#endif 


