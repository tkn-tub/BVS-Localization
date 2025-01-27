/*
 * PrintBiomarkers.cc
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

#include <unordered_set>    // Add this line to include the unordered_set header
#include <iostream>
#include <fstream>
#include "PrintBiomarkers.h"
#include "Bloodvessel.h"
#include "Biomarker.h"
#include "ns3/log.h" // Include for logging #include "ns3/log.h"  
#include "Constants.h" 


using namespace std;;
namespace ns3 {

PrintBiomarkers::PrintBiomarkers() {
    output.open("csvB007_40_20.csv", ios::out | ios::trunc); // Open the output CSV file
    outputM.open("csvBioM007_40.csv", ios::out | ios::trunc);
}
PrintBiomarkers::~PrintBiomarkers() {
    output.flush();  // Flush any remaining data to the file
    output.close();  // Close the file
    outputM.flush();  // Flush any remaining data to the file
    outputM.close();  // Close the file    
}


void PrintBiomarkers::PrintBiomarker(Ptr<Biomarker> b, int vesselID) 
{
    // cout << "In PrintBiomarkers::PrintBiomarker" << "\n";
    string id = b->GetBiomarkerID();
    double x = b->GetPosition().x;
    double y = b->GetPosition().y;
    double z = b->GetPosition().z;
    double currentSimTime = Simulator::Now().GetSeconds(); // Simulation time
    
    int BvID = vesselID;

    int st = b->GetStream();
    string type = b->GetType();
    string sourceData = b->GetSourceData();

    static std::unordered_set<std::string> seenBiomarkerIds94; 
    static std::unordered_set<std::string> seenBiomarkerIds75;
    static std::unordered_set<std::string> seenBiomarkerIds74;
    static std::unordered_set<std::string> seenBiomarkerIds67;
    static std::unordered_set<std::string> seenBiomarkerIds66;

    static double lastOutputTime = 0.0; // To track when last output was printed


    //cout << "Biomarker ID: " << id << " Position: " << x << ", " << y << ", " << z << ", Sim Time: " << currentSimTime << ", BvID: " 
    //       << BvID << ", Stream: " << st << ", Type: " << type << ", sourceData: " << sourceData << "\n";

    outputM << id << "," << x << "," << y << "," << z << "," << currentSimTime << "," 
           << BvID << "," << st << "," << type << "\n"; // Output to CSV

    //Ptr<NanoNetDevice> nanodev =  n->GetNanoNetDevice();
    Ptr<NetDevice> dev =  b->GetDevice();
    Ptr<BiomarkerNetDevice> biodev =  b->GetBiomarkerNetDevice();
    Ptr<BiomarkerChannel> biochannel = biodev->GetBiomarkerChannel(); 
        
    //cout << "In PrintBiomarker " << dev << " , " << biodev << " , " << biochannel << " , IN bloodvessel: " << BvID << "\n";
    
    if(vesselID == 94){
        seenBiomarkerIds94.insert(id); 
        // cout << "At 94, new biomarker: " << id << "\n";
    }
       
    if(vesselID == 75){
        seenBiomarkerIds75.insert(id); 
        // cout << "At 75, new biomarker: " << id << "\n";
    }
    
    if(vesselID == 74){ 
        seenBiomarkerIds74.insert(id);
        // cout << "At ===== 74, new biomarker: " << id << "\n";
    }
    
    if(vesselID == 67){
        seenBiomarkerIds67.insert(id); 
        // cout << "At 67, new biomarker: " << id << "\n";
    }
    
    if(vesselID == 66){ 
        seenBiomarkerIds66.insert(id);
        // cout << "At ===== 66, new biomarker: " << id << "\n";
    }
        
    // At the end of each simulation second:     
    if (currentSimTime - lastOutputTime >= 1.0) { // Check if a second has passed
        // Output to CSV
        //output << currentSimTime << "," << id << "," << seenBiomarkerIds94.size() << ", " << seenBiomarkerIds2.size() << "\n";
        output << currentSimTime << "," << id << "," 
                << seenBiomarkerIds94.size() << ", " 
                << seenBiomarkerIds75.size() << ", "
                << seenBiomarkerIds74.size() << ", "
                << seenBiomarkerIds67.size() << ", "              
                << seenBiomarkerIds66.size() << "\n";

        output.flush();
        // Print to console
        std::cout << "----------------- Total at 94: " << seenBiomarkerIds94.size() << std::endl;

        seenBiomarkerIds94.clear();
        seenBiomarkerIds75.clear();
        seenBiomarkerIds74.clear();
        seenBiomarkerIds67.clear();
        seenBiomarkerIds66.clear();

        //std::cout << "-After clearance.. Total at 94: " << seenBiomarkerIds94.size()  << ", At 2: " << seenBiomarkerIds2.size() << std::endl;       
        lastOutputTime = currentSimTime; // Update last output time 

    }
    /*
    // Check for Nearby Nanobots:
    std::list<int> nearbyNanobotIds = bloodvessel->IsNanobotInRange(b, ns3::COMMUNICATION_THRESHOLD);

    // Convert List to String:
    std::string nearbyNanobotsString = "";
    for (int nanobotId : nearbyNanobotIds) {
        nearbyNanobotsString += std::to_string(nanobotId) + " ";
        cout << "Biomarker ID: " << id << " Nearby nanobots: " << nearbyNanobotsString << "\n";
    }
    */

}

void PrintBiomarkers::PrintSomeBiomarkers(list<Ptr<Biomarker>> biomarkerList, int vesselID) 
{
    // cout << "In PrintSomeBiomarkers" << "\n";
    for (const Ptr<Biomarker>& biomarker : biomarkerList) {
        PrintBiomarker(biomarker, vesselID); // Print each biomarker in the list
    }
}

void PrintBiomarkers::PrintBiomarkersInTerminal(vector<Ptr<Bloodstream>> streamsOfVessel, int vesselIDl) 
{
    //cout << "In PrintBiomarkers::PrintBiomarkersInTerminal" << "\n";
    cout.precision(3);
    cout << "VESSEL ----------------" << vesselIDl << "--------" << endl;
    cout << "Time ----------------" << Simulator::Now() << "--------" << endl;

    for (uint j = 0; j < streamsOfVessel.size(); j++) {
        cout << "Stream " << j + 1 << " ------------------------" << endl;

        for (uint i = 0; i < streamsOfVessel[j]->CountBiomarkers(); i++) { // Iterate over biomarkers
            Ptr<Biomarker> b = streamsOfVessel[j]->GetBiomarker(i);
            cout << b->GetBiomarkerID() << ":" << b->GetPosition().x << ":" << b->GetPosition().y 
                 << ":" << b->GetPosition().z << ":" << b->GetSize() << ":" << b->GetType() << endl; // Add type
        }
        cout << "----------------------" << endl;
    }
}

} // namespace ns3