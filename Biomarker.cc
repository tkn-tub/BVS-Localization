/*
 * Biomarker.cc
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
 *  Created on: 2024. 03. 04.
 *      Author: Saswati Pal <saswati.pal@tu-berlin.de>
 */

#include "BiomarkerNetDevice.h" 
#include "Biomarker.h"
#include "Bloodvessel.h" 
#include "BiomarkerChannel.h"
#include "ns3/log.h" 
#include "ns3/simulator.h" // Include the simulator header for time functions

using namespace std;

namespace ns3 {

Biomarker::Biomarker() {
    // m_biomarkerID = 0; // for INTEGER
    m_biomarkerID = "";
    m_size = 10;     // Default size (10 µm in diameter set this in LocateInfectionSource)
    m_type = "";      // Default type (will be set later)
    m_sourceData = ""; // Default source data (will be set later)
    m_stream_bm = 0;  
    m_shouldChange = false; 
    m_creationTime = Simulator::Now();  // Set the creation time when the object is instantiated
    m_activeTime = Seconds(60.0);        // Default active time of 60 seconds (adjust as needed)

    bm_node = CreateObject<Node>();     // Create a node to hold the biomarker's position
    //enables mobility
    MobilityHelper mobility;
    // mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel"); // GaussMarkovMobilityModel
    mobility.Install(bm_node);
}


Biomarker::~Biomarker() {
    // No specific actions needed in the destructor for now
}

// Getter and Setter Implementations
string Biomarker::GetBiomarkerID() { 
    return m_biomarkerID; 
}
void Biomarker::SetBiomarkerID(string idbio) { 
    m_biomarkerID = idbio; 
}

double Biomarker::GetSize() { 
    return m_size; 
}
void Biomarker::SetSize(double size) { 
    m_size = size; 
}

string Biomarker::GetType() { return m_type; }
void Biomarker::SetType(string& type) { m_type = type; }

string Biomarker::GetSourceData() { return m_sourceData; }
void Biomarker::SetSourceData(string& data) { m_sourceData = data; }


int Biomarker::GetStream() { 
    return m_stream_bm; 
}

void Biomarker::SetStream(int streamId) { 
    m_stream_bm = streamId; 
}

void Biomarker::SetBloodvessel(Ptr<Bloodvessel> bloodvessel) {
   m_bloodvessel = bloodvessel;
}

Ptr<Bloodvessel> Biomarker::GetBloodvessel() {
   return m_bloodvessel;
}

bool Biomarker::GetShouldChange() { 
    return m_shouldChange; 
}

void Biomarker::SetShouldChange(bool shouldChangeStream) { 
    m_shouldChange = shouldChangeStream; 
}

ns3::Time Biomarker::GetTimeStep() { 
    return m_timeStep; 
}

void Biomarker::SetTimeStep() { m_timeStep = Simulator::Now(); }

Vector Biomarker::GetPosition() { 
    return bm_node->GetObject<MobilityModel>()->GetPosition(); 
}
void Biomarker::SetPosition(Vector posivalue) { 
    bm_node->GetObject<MobilityModel>()->SetPosition(posivalue); 
}

Time Biomarker::GetActiveTime() {
    return m_activeTime;
}

void Biomarker::SetActiveTime(Time activeTime) {
    m_activeTime = activeTime;
}

bool Biomarker::ShouldBeRemoved() {
    return Simulator::Now() - m_creationTime >= m_activeTime;
}

bool Biomarker::Compare(Ptr<Biomarker> b1, Ptr<Biomarker> b2) {
    // Compare the two biomarker IDs
    return b1->GetBiomarkerID() < b2->GetBiomarkerID();  // <: If b1's ID is smaller, the function returns true. Otherwise, it returns false.
}

// Install the communication device 
void Biomarker::InstallBiomarkerNetDevice(Ptr<BiomarkerChannel> channel) {
    Ptr<BiomarkerNetDevice> device = CreateObject<BiomarkerNetDevice>();

    // Set a unique MAC address for the device (if needed)
    device->SetAddress(Mac48Address::Allocate());

    // Associate the device with the biomarker's channel
    device->SetChannel(channel);  // OR The channel will be connected to the device in the Bloodcircuit.cc file.

    // Add the device to the biomarker's node
    bm_node->AddDevice(device);

    // Store a reference to the device
    m_biomarkerNetDevice = device;
}

Ptr<NetDevice> Biomarker::GetDevice() {
     return bm_node->GetDevice(0); // biomarker device is the first device added
 } 

// Return the pointer to the stored BiomarkerNetDevice object // Add a getter for the biomarker's network device
Ptr<BiomarkerNetDevice> Biomarker::GetBiomarkerNetDevice() {
    return m_biomarkerNetDevice; 
}

Ptr<Node> Biomarker::GetNode() {
    return bm_node; 
} 

}// namespace ns3  