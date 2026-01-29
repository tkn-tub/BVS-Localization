/*
 * BiomarkerNetDevice.h
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



#ifndef CLASS_BiomarkerNetDevice_
#define CLASS_BiomarkerNetDevice_

#include "BiomarkerChannel.h"
#include "NanoNetDevice.h" 
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/packet.h" 
#include "Biomarker.h"

namespace ns3 {
class BiomarkerChannel;
class Biomarker; //forward declaration

/**
* \brief BiomarkerNetDevice models the communication device on a biomarker.
*
* It is instantiated when the biomarker is created. It sends the biomarker's data (infection source) 
* to a nearby nanobot via the BiomarkerChannel's send() function.
*/

class BiomarkerNetDevice : public NetDevice {
public:
    /// Constructor
    BiomarkerNetDevice();

    /// Destructor
    ~BiomarkerNetDevice();

    /// Set the BiomarkerChannel to use for communication.
    void SetChannel(Ptr<BiomarkerChannel> channel);

    /// Get the BiomarkerChannel associated with this device.
    Ptr<BiomarkerChannel> GetBiomarkerChannel();

    /// Install this device on a node (presumably the biomarker's Node).
    void InstallToNode(Ptr<Node> node);

    void SetBiomarker(Ptr<Biomarker> biomarker);
   
    Ptr<Biomarker> GetBiomarker();

    //Ptr<Packet> GetPacket(); 

    // Receive data from a nanobot. (Optional if needed)
    /// void ReceiveData(Ptr<Packet> packet); 

    // inherited from NetDevice base class, are set to dummy values
    void SetIfIndex(const uint32_t index) override {};
    uint32_t GetIfIndex() const override { return 0; };
    Ptr<Channel> GetChannel() const override;
    void SetAddress(Address address) override;
    Address GetAddress() const override;
    bool SetMtu(const uint16_t mtu) override { return false; };
    uint16_t GetMtu() const override { return 0; };
    bool IsLinkUp() const override { return false; };
    void AddLinkChangeCallback(Callback<void> callback) override {};
    bool IsBroadcast() const override { return false; };
    Address GetBroadcast() const override { return Mac48Address("ff:ff:ff:ff:ff:ff"); };
    bool IsMulticast() const override { return false; };
    Address GetMulticast(Ipv4Address multicastGroup) const override
    { return Mac48Address("ff:ff:ff:ff:ff:ff"); };
    bool IsPointToPoint() const override { return false; };
    bool IsBridge() const override { return false; };
    bool Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) override;
    bool SendFrom(Ptr<Packet> packet,
                  const Address& source,
                  const Address& dest,
                  uint16_t protocolNumber) override;
                 
    Ptr<Node> GetNode() const override;
    void SetNode(Ptr<Node> node) override;
    bool NeedsArp() const override { return false; };
    void SetReceiveCallback(NetDevice::ReceiveCallback cb) override {};
    Address GetMulticast(Ipv6Address addr) const override { return Mac48Address::GetMulticast(addr); };
    void SetPromiscReceiveCallback(PromiscReceiveCallback cb) override {};
    bool SupportsSendFrom() const override { return false; };
    
private:
    Mac48Address m_address;         // MAC address of the device
    Ptr<BiomarkerChannel> m_channel; // Pointer to the biomarker communication channel
    //Ptr<Packet> packet;             // Pointer to a data packet (if used)
    Ptr<Node> m_node;               // Pointer to the node the device is attached to (Biomarker)

    // Pointer to the associated Biomarker
    Ptr<Biomarker> m_biomarker; 

    // Configurable communication threshold attribute
    /// double m_communicationThreshold;


};

} //end namespace
#endif