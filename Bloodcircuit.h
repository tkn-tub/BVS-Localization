/* -*-  Mode: C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Universität zu Lübeck [GEYER]
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
 * Author: Regine Geyer <geyer@itm.uni-luebeck.de>, Chris Deter <chris.deter@student.uni-luebeck.de>
 */

#ifndef CLASS_BLOODCIRCUIT_
#define CLASS_BLOODCIRCUIT_

#include "Bloodvessel.h"

using namespace std;

namespace ns3 {
/**
* \brief Bloodcircuit holds all Bloodvessels of the bodymodel of a female: hight 1.72m, weight 69kg, age 29.
*
* Each Bloodvessel is created and the Nanobots are injected into one specific Bloodvessel (default:
* Aorta_ascendens). The angle and length of each Bloodvessel is calculated with the coordinates.
*/
class Bloodcircuit
{
private:
  unsigned int valuesPerLine= 8;

  // A map of m_bloodvesselId to Bloodvessel.
  map<int, ns3::Ptr<ns3::Bloodvessel>> m_bloodvessels;

  Ptr<PrintNanobots> printer;

  Ptr<PrintBiomarkers> printBio;

  // add a new vessel Object to the bloodcircouit
  void AddVesselData (int id, BloodvesselType type, Vector start, Vector stop);

  // creates vessel objects from array
  void InitialiseBloodvessels (int vesseldata[][8], unsigned int count);

  // creates connections between bloodvessels
  void ConnectBloodvessels ();

  ///Calculates the normalized direction vector of a vessel.
  /**
  * \param bloodvessel pointer to the Bloodvessel.
  * \return the normalized direction vector of the Bloodvessel.
  */
  Vector CalcDirectionVectorNorm (Ptr<Bloodvessel> bloodvessel);

  /**
  * \param numberOfNanobots to be simulated.
  * \param bloodvessel where the Nanobots get injected.
  * \param gatewayPosition where the Nanobot transmit Message to Gateway.
  */
  void InjectNanobots (int numberOfNanobots, Ptr<Bloodvessel> bloodvessel, std::vector<int> gatewayPositions, std::vector<int> tissue_ID, float vesselthickness);

  /**
  * \param bloodvessel A pointer to the Bloodvessel object where the infection source will be located.
  * \param position The position within the Bloodvessel where the infection source will be placed...???  Vector position,      
  */
  void LocateInfectionSource(unsigned int simulationDuration, Ptr<Bloodvessel> bloodvessel);


  /**
 * \param type of Bloodvessel
 * \return speed of Bloodvesseltype
 */
  double GetSpeedClassOfBloodVesselType (BloodvesselType type);

  /// Return the Bloodcircuit map.
  map<int, ns3::Ptr<ns3::Bloodvessel>> GetBloodcircuit ();

public:
  /// The constructor setting up the Bloodcircuit.
  Bloodcircuit (unsigned int simulationDuration, unsigned int numberOfNanobots, unsigned int injectionVessel, Ptr<PrintNanobots> printer, Ptr<PrintBiomarkers> printBio, std::vector<int> gatewayPositions, std::vector<int> tissue_ID, float vesselthickness, std::vector<int> infection_source_vessel_ID);

  /// Destructor to clean up the map.
  ~Bloodcircuit ();

  static int BeginnSimulation (unsigned int simulationDuration, unsigned int numOfNanobots, unsigned int injectionVessel, std::vector<int> gatewayPositions, std::vector<int> tissue_ID, float vesselthickness, std::vector<int> infection_source_vessel_ID);

  static void releaseBiomarkers(Ptr<Bloodvessel> bloodvessel, Vector m_direction, std::string biomarkerType, std::string sourceData);

};
}; // namespace ns3
#endif