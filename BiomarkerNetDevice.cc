/*
 * NanoNetDevice.h
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

#include "BiomarkerNetDevice.h"
#include "BiomarkerChannel.h" 
#include "NanoNetDevice.h"
#include "ns3/log.h" 
#include "ns3/address.h"
#include "ns3/packet.h"
#include "ns3/node.h"  
#include "ns3/mobility-module.h" 
#include "Constants.h"
#include "Biomarker.h"
#include "Bloodvessel.h" 
#include "Nanobot.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("BiomarkerNetDevice");

// Constructor /// BiomarkerNetDevice::BiomarkerNetDevice()
BiomarkerNetDevice::BiomarkerNetDevice()
    : m_address(Mac48Address::Allocate()),
      m_channel(nullptr),
      m_node(nullptr),
      m_biomarker(nullptr) {} 

/**BiomarkerNetDevice::BiomarkerNetDevice(Ptr<Biomarker> biomarker) : m_biomarker(biomarker) 
{
    m_channel = nullptr;
    m_node = nullptr;
} **/

// Destructor
BiomarkerNetDevice::~BiomarkerNetDevice() {}

// Set the BiomarkerChannel
void BiomarkerNetDevice::SetChannel(Ptr<BiomarkerChannel> channel) {
    NS_LOG_FUNCTION(this);
    m_channel = channel;
}

// Get the BiomarkerChannel
Ptr<BiomarkerChannel> BiomarkerNetDevice::GetBiomarkerChannel() {
    NS_LOG_FUNCTION(this);
    return m_channel;
}

// Install the device to the node
void BiomarkerNetDevice::InstallToNode(Ptr<Node> node) {
    NS_LOG_FUNCTION(this);
    m_node = node;
}

Ptr<Biomarker> BiomarkerNetDevice::GetBiomarker() {
    return m_biomarker; // We have a member variable 'm_biomarker' in the BiomarkerNetDevice class to store the associated Biomarker
}

/** Ptr<Packet> BiomarkerNetDevice::GetPacket()
{
    return packet;  // Return the packet
} **/

// Transmit biomarker data over the channel
bool BiomarkerNetDevice::Send(Ptr<ns3::Packet> packet, const Address& dest, uint16_t protocolNumber) 
{
    return SendFrom(packet, m_address, dest, protocolNumber);
}

bool BiomarkerNetDevice::SendFrom(Ptr<ns3::Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << packet << source << dest << protocolNumber);
    // Ensure the Biomarker is correctly attached to a Bloodvessel
    Ptr<Bloodvessel> bloodvessel = m_node->GetObject<Biomarker>()->GetObject<Bloodvessel>();
    if (!bloodvessel) {
        NS_LOG_ERROR("BiomarkerNetDevice::SendFrom: Bloodvessel not found for biomarker.");
        return false;
    }
    Ptr<Biomarker> biomarker = m_node->GetObject<Biomarker>();
    
    // Get list of nearby nanobot IDs
    std::list<int> nearbyNanobotIds = bloodvessel->IsNanobotInRange(biomarker, ns3::COMMUNICATION_THRESHOLD);

    if (!nearbyNanobotIds.empty()) {
      // Use the channel to transmit the data to nearby nanobots
      m_channel->SendToNanobots(packet, biomarker);

      // Log successful transmission
      NS_LOG_INFO("Biomarker " << biomarker->GetBiomarkerID() << " sent data to nanobots in vessel "
                  << bloodvessel->GetbloodvesselID());
      return true;
  } else {
      NS_LOG_WARN("BiomarkerNetDevice: No nanobot in range for data transmission.");
      return false;
  }
}



// Override functions from the NetDevice base class (dummy implementations)
Ptr<Channel> BiomarkerNetDevice::GetChannel() const { 
    return m_channel; 
    }

void BiomarkerNetDevice::SetAddress(Address address) { 
    m_address = Mac48Address::ConvertFrom(address); 
    }

Address BiomarkerNetDevice::GetAddress() const { 
    return m_address; 
    }

Ptr<Node> BiomarkerNetDevice::GetNode() const { 
    return m_node; 
    }

void BiomarkerNetDevice::SetNode(Ptr<Node> node) { 
    m_node = node; 
    }

} // namespace ns3