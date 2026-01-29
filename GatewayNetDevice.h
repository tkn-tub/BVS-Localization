/*
 * GatewayNetDevice.h
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


#ifndef CLASS_GatewayNetDevice_
#define CLASS_GatewayNetDevice_

#include "BVSChannel.h"

using namespace std;

namespace ns3{
class BVSChannel;



/**
* \brief GatewayNetDevice models the communication device at each gateway
*
* It is instantiated when the channel is created and 
* receives the data from the nanobot via the channel function send() 
*/
class GatewayNetDevice : public NetDevice
{

public:
	//static TypeId GetTypeId(void);

    //constrcutor
	GatewayNetDevice();
    GatewayNetDevice(int pos);

    //deconstructor
	~GatewayNetDevice();

	//void Receive(Ptr<Packet> packet, uint16_t protocol, Mac48Address to, Mac48Address from);

    //receives a message by the nanobot and outputs it into stores and csv file
    void Receive(MAC_PHY_DATA *data, float vesselthickness);

    //calculate the ber and per
    int CalculateErrorBits(MAC_PHY_DATA *data);

    //specify which channel the gateway is part of
	void SetChannel(Ptr<BVSChannel> channel);

    //intall to a node 
	void installToNode(Ptr<Node> node);

    int getPosition();

	// inherited from NetDevice base class, set to dummy values
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



private :

	Mac48Address m_address;

    Ptr<BVSChannel> m_channel;

	Ptr<Node> m_node;

    vector<MAC_PHY_DATA> m_storage;

    int m_position;

};

}; //end namespace

#endif
