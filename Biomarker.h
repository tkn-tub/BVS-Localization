/*
 * Biomarker.h
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

#ifndef CLASS_BIOMARKER_
#define CLASS_BIOMARKER_

#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include <iostream>
#include <vector>
#include "BiomarkerNetDevice.h"
#include "BiomarkerChannel.h" // Include BiomarkerChannel

// Forward declaration of BiomarkerChannel
namespace ns3 {
class BiomarkerChannel;
class Bloodvessel;
}

namespace ns3 {
class BiomarkerNetDevice; 
class Biomarker;

/**
* \brief Biomarker is a mobile Object representing a biological indicator within a Bloodvessel (biological marker circulating in the bloodstream).
* Each Biomarker has a position within a Bloodvessel and can be detected by Nanobots.
* It carries information about its source (e.g., infection type).
* Biomarker has attributes such as position, size, and active time.
* A Biomarker belongs to a particular stream within a specific Bloodvessel.
* It will be removed from the simulation after a certain active time.
*/

class Biomarker : public ns3::Object {
private:
    string m_biomarkerID;      // Unique identifier for the biomarker
    // int m_biomarkerID;
    double m_size;         // Size of the biomarker
    string m_type;         // Type of biomarker, linked to infection source
    string m_sourceData;   // Data associated with the infection source

    Ptr<Bloodvessel> m_bloodvessel;
    int m_stream_bm;        // Stream within the blood vessel where the biomarker is located
    bool m_shouldChange;    // Flag indicating if the biomarker should change streams (might not be necessary)
    ns3::Time m_timeStep;     // Simulation time of the last change in position

    ns3::Time m_activeTime;          // Duration the biomarker remains active in the simulation
    ns3::Time m_creationTime;        // Time when the biomarker was created

    Ptr<Node> bm_node;           // Node representing the biomarker's position
    Ptr<BiomarkerNetDevice> m_biomarkerNetDevice;   // Communication device

    Ptr<MobilityModel> m_mobilityModel;  // Pointer to mobility model

public:

    /// Constructor to initialize values of all variables.
    /// The BiomarkerID is set in bloodcircuit, where the Biomarkers are initialized.
    Biomarker();

    /// Destructor 
    ~Biomarker(); 

    ///Getter and setter methods

    /**
    * \returns the Biomarker Id.
    */
    string GetBiomarkerID();

    /**
    * \param idbio a Biomarker Id.
    *
    * A Biomarker has a unique Id.
    */
    void SetBiomarkerID(string idbio);

    /**
     * \brief Retrieves the size of the biomarker.
     *
     * \return The size of the biomarker as a double value.
     */
    double GetSize(); 

    /**
     * \brief Sets the size of the biomarker.
     *
     * \param size The size to be assigned to the biomarker.
     */
    void SetSize(double size);

    /**
     * \brief Retrieves the type of the biomarker.
     *
     * The type is usually associated with the source of infection.
     * 
     * \return The type of the biomarker as a string.
     */
    string GetType(); 

    /**
     * \brief Sets the type of the biomarker.
     *
     * \param type The type to be assigned to the biomarker.
     */
    void SetType(string& type); 

    /**
     * \brief Retrieves the source data associated with the biomarker.
     *
     * This data might represent specific information about the infection source.
     *
     * \return The source data as a string.
     */
    string GetSourceData(); 

    /**
     * \brief Sets the source data for the biomarker.
     *
     * \param data The data to be associated with the biomarker.
     */
    void SetSourceData(string& data);

    /**
    * \brief Sets the Bloodvessel object that contains this biomarker.
    *
    * \param bloodvessel A pointer to the Bloodvessel object where this biomarker is located.
    */
    void SetBloodvessel(Ptr<Bloodvessel> bloodvessel);

    /**
    * \brief Gets the Bloodvessel object that contains this biomarker.
    *
    * \return A pointer to the Bloodvessel object where this biomarker is located.
    */
    Ptr<Bloodvessel> GetBloodvessel(); 

    /**
    * \returns the stream of the Biomarker.
    */
    int GetStream();

    /**
    * \param streamId the stream of the Biomarker.
    */
    void SetStream(int streamId);

    /**
    * \returns true if the Biomarker should change streams.
    */
    bool GetShouldChange(); // Optional, if you need stream changing behavior

    /**
    * \param shouldChangeStream True if the biomarker should change streams.
    */
    void SetShouldChange(bool shouldChangeStream);

    /**
    * \returns the time of the last change of Biomarker's position
    */
    ns3::Time GetTimeStep();

    /**
    * \sets the time of the last change in position.
    */
    void SetTimeStep();

    /**
    * \returns the position of Biomarker's Node which is located at the center of the Biomarker.
    */
    Vector GetPosition();

    /**
    * \param posivalue a position Vector.
    *
    * This function sets the position of Biomarker's Node. Biomarker's position is its Node's position.
    * The position Vector must point to the center of the Biomarker.
    */
    void SetPosition(Vector posivalue);

    /**
    * \param device a BiomarkerNetDevice pointer.
    *
    * This function install the BiomarkerNetDevice inside the Biomarker.
    */
    void InstallBiomarkerNetDevice(Ptr<BiomarkerChannel> channel); 
    
    /**
    * \returns the pointer to the installed Netdevice
    */
    Ptr<NetDevice> GetDevice(); 

   
    /**
    * \brief Gets a pointer to the installed BiomarkerNetDevice.
    *
    * \return A pointer to the BiomarkerNetDevice object associated with this biomarker.
    */
    Ptr<BiomarkerNetDevice> GetBiomarkerNetDevice(); 

    /**
     * \returns the node of the Biomarker.
     */
    Ptr<Node> GetNode();

    
    /**
     * \return The active time of the biomarker.
     */
    ns3::Time GetActiveTime();

    /**
     * \param activeTime The duration the biomarker remains active.
     *
     * Sets the active time of the biomarker.
     */
    void SetActiveTime(Time activeTime);

    /**
     * \return True if the biomarker should be removed from the simulation.
     *
     * This function checks if the current simulation time has exceeded the biomarker's 
     * creation time plus its active time.
     */
    bool ShouldBeRemoved(); 

    // Install a mobility model for the biomarker
    // void InstallMobilityModel(Ptr<MobilityModel> mobilityModel); 

    static bool Compare (Ptr<Biomarker> v1, Ptr<Biomarker> v2);

}; // Class End
} // namespace ns3
#endif