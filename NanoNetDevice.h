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
 *  Created on: 2023. 12. 6.
 *      Author: Laurenz Ebner
 */



#ifndef CLASS_NanoNetDevice_
#define CLASS_NanoNetDevice_

#include "BVSChannel.h"
#include "GatewayNetDevice.h"
#include "BiomarkerNetDevice.h"  // Include BiomarkerNetDevice header to receive data from Biomarkers
#include "BiomarkerChannel.h"


namespace ns3 {
class BiomarkerNetDevice;
class BiomarkerChannel;    
class BVSChannel;

/**
* \brief NanoNetDevice models the communication device at each nanobot 
*
* It is instantiated when the nanobot is created, creates random data each time it passes the tissue_ID and 
* sends the data to the gateway via the channel function send() 
* EDIT: It can also receive data from nearby biomarkers through the BiomarkerChannel.
*/

class NanoNetDevice : public NetDevice
{

public:
    //static TypeId GetTypeId (void);

    //constructor
    NanoNetDevice ();

    //NanoNetDevice(Ptr<BVSChannel> Channel);

    //deconstructor
    ~NanoNetDevice ();

    //specify the channel pointer where to transmit in
    void SetChannel(Ptr<BVSChannel> channel);

    //get the channel pointer
    Ptr<BVSChannel> GetBVSChannel();

    // EDIT: Function to receive data from a biomarker
    void ReceiveDataFromBiomarker(Ptr<Packet> packet, Ptr<BiomarkerNetDevice> biomarkerDevice);

    //get the channel pointer
    Ptr<BiomarkerChannel> GetBiomarkerChannel();   

    /**
     * \brief Sets the BiomarkerChannel for the NanoNetDevice.
     *
     * \param channel A pointer to the BiomarkerChannel object.
     */
    void SetBiomarkerChannel(Ptr<BiomarkerChannel> biochannel); 


    //create the measurement data
    void createMacPhyData(int tissue_ID, int nanobot_ID);

    //get the transmission package 
    MAC_PHY_DATA *getMacPhyData();

    //intall to the node inside of the nanobot class
    void installToNode(Ptr<Node> node);
    
    // inherited from NetDevice base class, are set to dummy values 
    void SetIfIndex(const uint32_t index) override{}; 
    uint32_t GetIfIndex() const override{return 0;};
    Ptr<Channel> GetChannel() const override;
    void SetAddress(Address address) override;
    Address GetAddress() const override;
    bool SetMtu(const uint16_t mtu) override{return false;};
    uint16_t GetMtu() const override{return 0;};
    bool IsLinkUp() const override{return false;};
    void AddLinkChangeCallback(Callback<void> callback) override{};
    bool IsBroadcast() const override{return false;};
    Address GetBroadcast() const override{return Mac48Address("ff:ff:ff:ff:ff:ff");};
    bool IsMulticast() const override{return false;};
    Address GetMulticast(Ipv4Address multicastGroup) const override
    {return Mac48Address("ff:ff:ff:ff:ff:ff");};
    bool IsPointToPoint() const override{return false;};
    bool IsBridge() const override{return false;};
    bool Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) override;
    bool SendFrom(Ptr<Packet> packet,
                  const Address& source,
                  const Address& dest,
                  uint16_t protocolNumber) override;
    Ptr<Node> GetNode() const override;
    void SetNode(Ptr<Node> node) override;
    bool NeedsArp() const override{return false;};
    void SetReceiveCallback(NetDevice::ReceiveCallback cb) override{};

    Address GetMulticast(Ipv6Address addr) const override{return Mac48Address::GetMulticast(addr);};

    void SetPromiscReceiveCallback(PromiscReceiveCallback cb) override{};
    bool SupportsSendFrom() const override{return false;};

private:

    Mac48Address m_address;
    Ptr<BVSChannel> m_channel;
    Ptr<Packet> packet;
    Ptr<Node> m_node;
    MAC_PHY_DATA *m_mac_phy_data;
    // EDIT: Add a channel for biomarker communication
    Ptr<BiomarkerChannel> m_biomarkerChannel; 
};

}; //end namespace

#endif

