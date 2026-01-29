/*
 * PrintBiomarkers.h
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
#ifndef CLASS_PRINTBIOMARKERS_
#define CLASS_PRINTBIOMARKERS_

#include <unordered_set>    // Add this line to include the unordered_set header
#include <iostream>
#include "Bloodstream.h" // Include Bloodstream for accessing biomarker data
#include <fstream>
#include <list>

using namespace std;
namespace ns3 {
/**
* \brief PrintBiomarkers class for writing biomarker information to a CSV file.
*
* This class provides functions to print individual biomarkers and groups of biomarkers
* in a structured CSV format for analysis and visualization.
*/
class PrintBiomarkers : public ns3::Object
{
private:
    ofstream output; // Output file stream
    ofstream outputM;  // Output file stream for mobility

    std::unordered_set<std::string> seenBiomarkerIds94; 
    std::unordered_set<std::string> seenBiomarkerIds75;
    std::unordered_set<std::string> seenBiomarkerIds74; 
    std::unordered_set<std::string> seenBiomarkerIds67;
    std::unordered_set<std::string> seenBiomarkerIds66;  

public:
    PrintBiomarkers ();  // Constructor

    ~PrintBiomarkers (); // Destructor

    /**
     * \brief Prints information about a single biomarker to the CSV file.
     *
     * \param b Pointer to the Biomarker object.
     * \param vesselID The ID of the blood vessel the biomarker is in.
     */
    void PrintBiomarker (Ptr<Biomarker> b, int vesselID);

    /**
     * \brief Prints information about multiple biomarkers to the CSV file.
     *
     * \param biomarkerList A list of pointers to Biomarker objects.
     * \param vesselID The ID of the blood vessel the biomarkers are in.
     */
    void PrintSomeBiomarkers (list<Ptr<Biomarker>> biomarkerList, int vesselID);

    /**
     * \brief Prints biomarker information to the terminal.
     * (For debugging purposes)
     *
     * \param streamsOfVessel A vector of Bloodstream pointers.
     * \param vesselID The ID of the blood vessel.
     */
    void PrintBiomarkersInTerminal (vector<Ptr<Bloodstream>> streamsOfVessel, int vesselIDl);

}; // Class End
} // namespace ns3

#endif