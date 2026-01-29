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
 * Author: Regine Geyer <geyer@itm.uni-luebeck.de>
 */

#include "ns3/Bloodcircuit.h"

using namespace ns3;
using namespace std;

#define ITERATIONS 50

int
main (int argc, char *argv[])
{
  //Add default values:
  int numOfNanobots = 100;
  int simulationDuration = 500;
  int injectionVessel = 29; // 29 corresponds to left arm
  bool typeofsimulation = 0; // 0 is for single run, 1 is for sweep over tissue thickness
  int gatewayposition = 1;
  int tissue_ID_init = 94;
  float vesselthickness = 0.00036; // for single run, tissue thickness in [m]
  vector<int> gatewayPositions;
  vector<int> tissue_ID;
  bool isDeterministic = false;
  int num_infection_source = 1; 
  // azygos vein (87) is much closer to a probable breast cancer location
  vector<int> infection_source_vessel_ID = {18}; // 2, 66 // 24 for THORAX // 36 for LIVER // 56 KNEE
 // 59: left foot //////// 9: Head ///// 18. (Breast) A intercoastales posterior 
 // 40. Kidney
 // Lateral mammary branches – originate from the posterior intercostal arteries (derived from the aorta). 
 // They supply the lateral aspect of the breast in the 2nd 3rd and 4th [intercostal spaces.

  // check input parameters
  CommandLine cmd;
  cmd.AddValue ("simulationDuration", "simulationDuration", simulationDuration);
  cmd.AddValue ("numOfNanobots", "numOfNanobots", numOfNanobots);
  cmd.AddValue ("injectionVessel", "injectionVessel", injectionVessel);
  cmd.AddValue ("typeofsimulation","typeofsimulation", typeofsimulation);
  cmd.AddValue ("gatewayposition", "gatewayposition", gatewayposition);
  cmd.AddValue ("tissue_ID", "tissue_ID", tissue_ID_init);
  cmd.AddValue ("isDeterministic", "isDeterministic", isDeterministic);

  //......Get the number of infection sources from the user...........
  cmd.AddValue("num_infection_source", "Number of infection sources", num_infection_source);

  cmd.Parse (argc, argv);

  // add position of gateway and tissue_ID
  gatewayPositions.push_back(gatewayposition);
  tissue_ID.push_back(tissue_ID_init);

  std::cout << "Infection Source Vessel IDs: ";
  for (int vesselID : infection_source_vessel_ID) {
      std::cout << vesselID << " ";  
  }
  std::cout << std::endl;

  /*
  // Ask for vessel IDs based on the number of infection sources
  for (int i = 0; i < num_infection_source; ++i) {
      int vesselID;
      cout << "Enter vessel ID for infection source " << i + 1 << ": ";
      cin >> vesselID;
      infection_source_vessel_ID.push_back(vesselID);
  }
  //............................................... 
  */

  // check wether type of simulation is sweep or single run
   // clear csv files for writing 

  remove("gateway.csv");
  remove("gateway_amplitudes.csv");
  if (!typeofsimulation)
    remove("gateway_ber.csv");

 
  if (typeofsimulation)   // sweep
  {
    vector<float> vesselStep(ITERATIONS,0);

    float startval = 200*pow(10,-6);
    
    vesselStep[0] = startval;
    vesselStep[ITERATIONS-1] = 1800*pow(10,-6);

    float a = (vesselStep[ITERATIONS-1] - vesselStep[0]) / ITERATIONS;

    //create equally spaced vector of tissue thickness
    generate(vesselStep.begin(),vesselStep.end(), [n = 0, &a] () mutable { return a * n++; }); //create equally spaced vector of tissue thickness

    for (int i = 0; i < ITERATIONS; i++){
      vesselStep[i] = vesselStep[i] + startval;
    }

  // Call each simulation ITERATION times
    for (int i = 0; i < ITERATIONS; i++){
      Bloodcircuit::BeginnSimulation (simulationDuration, numOfNanobots, injectionVessel, gatewayPositions, tissue_ID, vesselStep[i], infection_source_vessel_ID, isDeterministic);
    }

    return 0;
  }

  // single run
  return Bloodcircuit::BeginnSimulation (simulationDuration, numOfNanobots, injectionVessel, gatewayPositions, tissue_ID, vesselthickness, infection_source_vessel_ID, isDeterministic);
}