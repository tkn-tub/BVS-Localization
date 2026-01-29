/*
 * BiomarkerChannel.cc
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

#include "BiomarkerChannel.h"
#include "ns3/log.h" 
#include "ns3/simulator.h"
#include "Bloodvessel.h" // Bloodvessel class to get streams and nanobots
#include "Constants.h"

// Include headers 
#include "BiomarkerNetDevice.h"
#include "NanoNetDevice.h" 
#include "Biomarker.h"

 /**
* \brief SendToNanobots function 
* The implementation iterate over all nanobots in the simulation, 
* check their distance to the biomarker, 
* and send the data packet to those within the communication range.
 */

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("BiomarkerChannel");

BiomarkerChannel::BiomarkerChannel() {}

BiomarkerChannel::~BiomarkerChannel() {}


void BiomarkerChannel::AddBiomarkerDevice(Ptr<BiomarkerNetDevice> device) {
    m_biomarkerDevices.push_back(device);
}


size_t BiomarkerChannel::GetNDevices (void) const
{
  return m_biomarkerDevices.size();
}


Ptr<NetDevice> BiomarkerChannel::GetDevice (std::size_t i) const
{
  return m_biomarkerDevices[i];
}

void BiomarkerChannel::SendToNanobots(Ptr<Packet> packet, Ptr<Biomarker> biomarker) {
    NS_LOG_FUNCTION(this << packet << biomarker);
    if (biomarker == nullptr || packet == nullptr) {
        std::cout << "Biomarker or packet is null in SendToNanobots " << std::endl;
        return;
    }
    // Get the blood vessel where the biomarker is located
    Ptr<Bloodvessel> bloodvessel = biomarker->GetObject<Bloodvessel>();
    // Check if the blood vessel exists and has streams
    if (!bloodvessel || bloodvessel->GetNumberOfStreams() == 0) {
        std::cout << "Bloodvessel not found or has no streams in SendToNanobots" << std::endl;
        return;
    }
    // Get BiomarkerNetDevice
    Ptr<BiomarkerNetDevice> biomarkerDevice = biomarker->GetBiomarkerNetDevice(); // Get the BiomarkerNetDevice 
    // Get the stream ID the biomarker is in
    int streamId = biomarker->GetStream();
    // Find nearby nanobots
    std::list<int> nearbyNanobotIds = bloodvessel->IsNanobotInRange(biomarker, ns3::communicationThreshold);
    for (int nanobotId : nearbyNanobotIds) 
    {   // Retrieve the nanobot from the bloodvessel using its ID
        Ptr<Nanobot> nanobot = bloodvessel->GetStream(streamId)->GetNanobot(nanobotId);

        // Check if the nanobot exists in the bloodstream
        if (nanobot != nullptr) {
            // Transmit the packet to the nanobot's device
            Ptr<NanoNetDevice> nanoDevice = nanobot->GetNanoNetDevice();
            nanoDevice->ReceiveDataFromBiomarker(packet->Copy(), biomarkerDevice); 

            cout << "Biomarker " << biomarker->GetBiomarkerID() << " sent data to Nanobot " 
                        << nanobot->GetNanobotID() << " in vessel " << bloodvessel->GetbloodvesselID() << "\n";
        } else {
            cout << "Nanobot with ID " << nanobotId << " not found in the bloodstream." << "\n";
        }
    }
}

} // namespace ns3




