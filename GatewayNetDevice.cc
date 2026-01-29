/*
 * GatewayNetDevice.cc
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


#include "GatewayNetDevice.h"

namespace ns3{

GatewayNetDevice::GatewayNetDevice() {

	m_channel = nullptr;

	m_node = nullptr;

    m_position = 0;

    vector<MAC_PHY_DATA> m_storage;

}

GatewayNetDevice::GatewayNetDevice(int pos) {

	m_channel = nullptr;

	m_node = nullptr;

    m_position = pos;

    vector<MAC_PHY_DATA> m_storage;
}

GatewayNetDevice::~GatewayNetDevice() {

}
/*
TypeId GatewayNetDevice::GetTypeId(void) {
	static TypeId tid = TypeId ("ns3::GatewayNetDevice").SetParent<Object> ().AddConstructor<GatewayNetDevice> ();
	return tid;
}
*/

void GatewayNetDevice::Receive(MAC_PHY_DATA *data, float vesselthickness)
{

    for(int i = 0; i < TESTPACKETSIZE; i++)
    {
        if ((*data).SEQ_RX[i] < 0)
        (*data).PDU_RX[i] = 0;
        else
        (*data).PDU_RX[i] = 1;
    }
    
   //uncomment for terminal output
   /* 
    cout << "dec : | ";

    for(int i = 0; i < TESTPACKETSIZE; i++)
    {
        cout << (*data).PDU_RX[i] << " | ";
    }

    cout << "\n";
    */
    m_storage.push_back((*data));

    fstream afout;

    afout.open("gateway_amplitudes.csv", ios::out | ios::app);

    for (double i : data->SEQ_RX)
            afout << i << ",";

    afout << "\n";

    fstream fout;

    fout.open("gateway.csv", ios::out | ios::app); 

    fout  << data->nanobot_ID << "; "
            << data->tissue_ID << "; ";

    for (int i : data->PDU_TX)
            fout << i << ",";

    fout << "; ";

    for (double i : data->SEQ_TX)
            fout << i << ",";

    fout << "; ";

    for (double i : data->SEQ_RX)
            fout << i << ",";
            
    fout << "; ";

    for (int i : data->PDU_RX)
            fout << i << ",";

    fout << "; ";

    fout << "\n";


    int be = CalculateErrorBits(data);

    int pe = 0;

    fstream foutber;

    foutber.open("gateway_ber.csv", ios::out | ios::app); 

    foutber <<  be << ",";

    foutber << size(data->PDU_RX) << ",";

    if (be > 0)
        pe = 1;

    foutber << pe << ",";

    foutber << vesselthickness << "\n";
}

int GatewayNetDevice::CalculateErrorBits(MAC_PHY_DATA *data)
{
int retval = 0;

for(int i = 0; i < TESTPACKETSIZE; i++)
{
    if(data->PDU_RX[i] != data->PDU_TX[i])
        retval++;

}

return retval;


}

Address GatewayNetDevice::GetAddress() const
     {
        return m_address;
     }

void GatewayNetDevice::SetAddress(Address address)
    {
        m_address = Mac48Address::ConvertFrom(address);
    }

Ptr<Channel> GatewayNetDevice::GetChannel() const
    {
        return m_channel;
    }


void GatewayNetDevice::SetChannel(Ptr<BVSChannel> channel) 
	{
		m_channel = channel;
		m_channel->Add (this);
	}

Ptr<Node> GatewayNetDevice::GetNode() const
    {
        return m_node;
    }

    void GatewayNetDevice::SetNode(Ptr<Node> node)
    {
        m_node = node;
    }

    bool GatewayNetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) 
    {
        return SendFrom(packet, m_address, dest, protocolNumber);
    }

    bool GatewayNetDevice::SendFrom(Ptr<Packet> packet,
                  const Address& source,
                  const Address& dest,
                  uint16_t protocolNumber)
    {
        // to be initliazed

        return false;
    }

    void GatewayNetDevice::installToNode(Ptr<Node> node)
    {
        node->AddDevice(this);
    }

    int GatewayNetDevice::getPosition()
    {
        return m_position;
    }

}


