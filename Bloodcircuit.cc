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

#include "Bloodcircuit.h"

#include "BiomarkerNetDevice.h"
#include "BiomarkerChannel.h"
#include "PrintBiomarkers.h"

#include "Constants.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <iostream>
#include <random>   // For random number generation
#include <cstdlib> // For srand and rand functions
#include <ctime>
#include <bitset> 
#include <chrono>
#include <thread>

/**
 * This function sets the Bloodvessel map of the human body of a female: hight 1.72m, weight 69kg, age 29.
 */

namespace ns3 {

Bloodcircuit::Bloodcircuit (unsigned int simulationDuration, unsigned int numberOfNanobots, unsigned int injectionVessel, Ptr<PrintNanobots> printer, Ptr<PrintBiomarkers> printBio, 
 std::vector<int> gatewayPositions, std::vector<int> tissue_ID, float vesselthickness, std::vector<int> infection_source_vessel_ID)
{
  // Seed the random number generator (doing this once at the beginning of the program)
  srand(time(0));
  // initialise map with bloodvesselinformation
  m_bloodvessels = map<int, Ptr<Bloodvessel>> ();
  this->printer = printer;
  // EDIT 
  this->printBio = printBio;
  // loading vasculature via csv
  std::ifstream infile{"vasculature.csv"};
  if (infile.good ())
    {
      std::vector<int> numbers;
      numbers.resize (valuesPerLine);
      std::string buffer;
      buffer.reserve (64);
      int errorflag = 0;
      while (infile.good ())
        {
          for (auto &&elem : numbers)
            {
              if (std::getline (infile, buffer, ','))
                {
                  elem = std::stoi (buffer);
                }
              else
                {
                  elem = 0;
                  errorflag = 1;
                }
            }
          if (errorflag == 0)
            {
              AddVesselData (numbers[0], (BloodvesselType) numbers[1],
                             Vector (numbers[2], numbers[3], numbers[4]),
                             Vector (numbers[5], numbers[6], numbers[7]));
            }
        }
      cout << "load CSV - done" << endl;
    }
  else
    { // Old way of loading data
      cout << "NO VALID CSV FILE FOUND! " << endl;
      cout << "please provide a valid 'vasculatures.csv'" << endl;
    }

  //Create Bloodcircuit with all Bloodvessels.
  //
  ConnectBloodvessels ();
  //Inject Nanobots here!
  //Places x Bots, randomly on Streams at specific vessel injected.
  //If you choose other values, the nanobots all get injected at the same coordinates

  if (m_bloodvessels.size() > 1){
    InjectNanobots (numberOfNanobots, m_bloodvessels[injectionVessel <  m_bloodvessels.size() ?  injectionVessel : m_bloodvessels.size() -1],gatewayPositions, tissue_ID, vesselthickness);
    // EDIT.....................Iterate through the infection source vessel IDs
    for (int vesselID : infection_source_vessel_ID) { 
        // Check if the vessel ID is valid within your m_bloodvessels map
        if (m_bloodvessels.count(vesselID) > 0) { 
            Ptr<Bloodvessel> bloodvessel = m_bloodvessels[vesselID];

            cout << "......... Calling LocateInfectionSource ........."  << endl; 
            // Call LocateInfectionSource
            LocateInfectionSource(simulationDuration, bloodvessel);
        } 
        else {
            // Handle invalid vessel IDs (optional)
            std::cout << "Error: Invalid vessel ID: " << vesselID << std::endl;
        }
    }     
  }
}

//Starts the simulation in a bloodvessel
void
Starter (Ptr<Bloodvessel> vessel)
{
  vessel->Start ();
}

int
Bloodcircuit::BeginnSimulation (unsigned int simulationDuration, unsigned int numOfNanobots, unsigned int injectionVessel, std::vector<int> gatewayPositions, std::vector<int> tissue_ID, float vesselthickness, 
 std::vector<int> infection_source_vessel_ID)
{
  //execution time
  clock_t start, finish;
  start = clock ();
  Ptr<PrintNanobots> printNano = new PrintNanobots (); 
  Ptr<PrintBiomarkers> printBio = new PrintBiomarkers();  // EDIT..................... 
  // Record start time
  auto startTime = std::chrono::high_resolution_clock::now();  

  // .....................   //Create the bloodcircuit
  cout << "......... Creating the bloodcircuit ........."  << endl;
  Bloodcircuit circuit (simulationDuration, numOfNanobots, injectionVessel, printNano, printBio, gatewayPositions, tissue_ID, vesselthickness, infection_source_vessel_ID);

  //Get the map of the bloodcircuit
  map<int, ns3::Ptr<ns3::Bloodvessel>> circuitMap = circuit.GetBloodcircuit ();
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  cout << "circuitMap.size(): " << circuitMap.size() << endl;
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  if(circuitMap.size() > 1)
  {        
  // Schedule and run the Simulation in each bloodvessel
    for (unsigned int i = 1; i < circuitMap.size() + 1; i++)
      {
                Simulator::Schedule (Seconds (0.0), &Starter, circuitMap[i]); 
      }  
  

    //Stop simulation after specific time
    Simulator::Stop (Seconds (simulationDuration + 1));
    Simulator::Run ();
    Simulator::Destroy ();
    //output execution time
    finish = clock ();
    cout << "Dauer: " << simulationDuration << "s " << numOfNanobots << "NB -> "
        << (finish - start) / 1000000 << "s ------------------------" << endl;
    cout << "Injected Vessel: " << injectionVessel << endl;

    // Record end time and calculate duration
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    // EDIT .....................    // Output execution time
    std::cout << "Duration: " << simulationDuration << "s, " << numOfNanobots << " nanobots, "
              << duration.count() / 1000000.0 << "s\n"; // Convert to seconds

    std::cout << "Injected Vessel: " << injectionVessel << std::endl;
    // .....................
    return 0;
  } else {
     cout << "Error: Not enough vessels for simulation! Please check 'vasculature.csv'" << endl;
     return 1; // Indicate an error occurred
  }
}

Bloodcircuit::~Bloodcircuit ()
{
  m_bloodvessels.clear ();
}

map<int, ns3::Ptr<ns3::Bloodvessel>>
Bloodcircuit::GetBloodcircuit ()
{
  return m_bloodvessels;
}

Vector
Bloodcircuit::CalcDirectionVectorNorm (Ptr<Bloodvessel> m_bloodvessel)
{
  Vector start = m_bloodvessel->GetStartPositionBloodvessel ();
  Vector end = m_bloodvessel->GetStopPositionBloodvessel ();
  double x = end.x - start.x;
  double y = end.y - start.y;
  double z = end.z - start.z;
  double vectorLength = sqrt (pow (x, 2) + pow (y, 2) + pow (z, 2));
  x = x / vectorLength;
  y = y / vectorLength;
  z = z / vectorLength;
  return Vector (x, y, z);
}

void
Bloodcircuit::InjectNanobots (int numberOfNanobots, Ptr<Bloodvessel> bloodvessel,std::vector<int> gatewayPositions, std::vector<int> tissue_ID, float vesselthickness)
{
  int nanobotGroupSize = 10;
  Ptr<UniformRandomVariable> distribute_randomly = bloodvessel->getRandomObjectBetween (0, bloodvessel->GetNumberOfStreams ());
  Vector m_coordinateStart = bloodvessel->GetStartPositionBloodvessel ();
  int intervall =
      (numberOfNanobots >= nanobotGroupSize) //IF if equal or more than 10 Nanobots are injected,
          ? div (numberOfNanobots, nanobotGroupSize)
                .quot // THEN Divide number of Nanobots into 10 equaly sized groups + remainder
          : numberOfNanobots; // ELSE put them on beginning of the bloodvessel in one point.
  //Calculate the normalized direction vector of the start vessel.
  Vector m_direction = CalcDirectionVectorNorm (bloodvessel);
  //Set direction intervall as 1/10 of the normalized direction vector.
  Vector directionIntervall =
      Vector (m_direction.x / nanobotGroupSize, m_direction.y / nanobotGroupSize,
              m_direction.z / nanobotGroupSize);
  unsigned int group = 0; // Group 0 to 9

  //create one BVSchannel per bloodvessel for transmission
  Ptr<BVSChannel> channel = CreateObject<BVSChannel> (vesselthickness);
  //connect the BVSChannel to the GatewayNetDevice
  for (int i = 0; i <= (int) gatewayPositions.size(); i++)
  {
    Ptr<GatewayNetDevice> gdev = CreateObject<GatewayNetDevice> (gatewayPositions[i]);
    channel->AddGateway(gdev);
  }

  //Distribute Nanobots in 10 groups over the beginning of the start vessel.
  for (int i = 1; i <= numberOfNanobots; ++i)
    {
      group = (i - 1) / intervall;

      Ptr<Nanobot> temp_nb = CreateObject<Nanobot> ();
      temp_nb->SetNanobotID (i);
      temp_nb->SetGatewayPositions(gatewayPositions);
      temp_nb->Settissue_ID(tissue_ID);
      temp_nb->InstallNanoNetDevice(channel);
      //Get random stream number.
      int dr = floor (distribute_randomly->GetValue ());
      temp_nb->SetShouldChange (false);
      temp_nb->SetPosition (Vector (m_coordinateStart.x + (directionIntervall.x * group),
                                    m_coordinateStart.y + (directionIntervall.y * group),
                                    m_coordinateStart.z + (directionIntervall.z * group)));
      //Set position with random stream dr.
      bloodvessel->AddNanobotToStream (dr, temp_nb);
    }
  //Print Nanobots in csv-file.
  bloodvessel->PrintNanobotsOfVessel ();
}

// EDIT ...................................................................................
void Bloodcircuit::LocateInfectionSource(unsigned int simulationDuration, Ptr<Bloodvessel> bloodvessel) 
{
    // Calculate the normalized direction vector of the blood vessel.
    Vector m_direction = CalcDirectionVectorNorm (bloodvessel);
    // Generate binary representation of the vessel ID as source data // Get the ID of the blood vessel and convert it to a bitset of size 32
    std::string sourceData = std::bitset<16>(bloodvessel->GetbloodvesselID()).to_string();
    // Create biomarker type string with "BM__" prefix
    std::string biomarkerType = "BM" + std::to_string(bloodvessel->GetbloodvesselID());
    // Print the generated biomarker type and source data to the terminal
    std::cout << "Biomarker type: " << biomarkerType << std::endl;
    std::cout << "Biomarker source data : " << sourceData << std::endl;

    // Initial release at the beginning of the simulation  // Call as a static member function
    Bloodcircuit::releaseBiomarkers(bloodvessel, m_direction, biomarkerType, sourceData);  

    // Schedule the release of BIOMARKERS     // Schedule subsequent biomarker releases every burstInterval seconds
    cout << "......... Scheduling biomarker release ........."  << endl;    

    int burstInterval = 1; // Biomarkers to release after burstInterval (every __ seconds)
    std::cout << "burstInterval: " << burstInterval << std::endl;
    
    for (double time = 1; time < simulationDuration; time += burstInterval) 
    {
        Simulator::Schedule(Seconds(time), &Bloodcircuit::releaseBiomarkers, bloodvessel, m_direction, biomarkerType, sourceData);    
    }   
}

void Bloodcircuit::releaseBiomarkers(Ptr<Bloodvessel> bloodvessel, Vector m_direction, std::string biomarkerType, std::string sourceData) 
{    
      std::random_device rd;  // Seed for the random number engine
      std::mt19937 gen(rd()); // Mersenne Twister engine initialized with rd()
      std::uniform_int_distribution<> dis(10, 50); // Define the range (1000, 5000)

      int burstIntensity = 100; // dis(gen); // Generate the random number            
  
      // Biomarker group size (optional for distribution)
      int biomarkerGroupSize = 4;
      // Initialize random number generator for stream selection
      Ptr<UniformRandomVariable> distributeRandomly = bloodvessel->getRandomObjectBetween(0, bloodvessel->GetNumberOfStreams());
      // Get the start position of the blood vessel
      Vector m_coordinateStart = bloodvessel->GetStartPositionBloodvessel(); 

      // std::cout << "m_direction: " << m_direction << std::endl;

      // Calculate interval for biomarker distribution (if grouping)
      int interval = (burstIntensity >= biomarkerGroupSize) 
                      ? div(burstIntensity, biomarkerGroupSize).quot 
                      : burstIntensity; 

      // Direction interval for biomarkers
      Vector directionInterval = Vector(m_direction.x / biomarkerGroupSize,
                                      m_direction.y / biomarkerGroupSize,
                                      m_direction.z / biomarkerGroupSize);
      unsigned int group = 0; // Group 0 to 9

      // Create a dedicated channel for biomarker-nanobot communication
      Ptr<BiomarkerChannel> biomarkerChannel = CreateObject<BiomarkerChannel>(); 

      // Connect the BiomarkerChannel to all NanoNetDevice instances in the current bloodvessel
      for (int streamId = 0; streamId < bloodvessel->GetNumberOfStreams(); ++streamId) 
      {
        // Dereference the pointer to get the Bloodstream object
        Ptr<Bloodstream> stream = bloodvessel->GetStream(streamId); 
        for (uint32_t j = 0; j < stream->CountNanobots(); j++) 
        {
          Ptr<Nanobot> nanobot = stream->GetNanobot(j);
          Ptr<NanoNetDevice> nanoNetDevice = nanobot->GetNanoNetDevice();
          // Connect the NanoNetDevice to the BiomarkerChannel
          nanoNetDevice->SetBiomarkerChannel(biomarkerChannel);
        }
      }
      // Distribute Biomarkers
      for (int i = 0; i < burstIntensity; i++) 
      {
          group = (i - 1) / interval; // Determine the group for the biomarker
          ///Ptr<Biomarker> biomarker = CreateObject<Biomarker>();
          Ptr<Biomarker> temp_bm = CreateObject<Biomarker>(); 

          // Construct the unique ID as a string (id+time)
          double nowTime = Simulator::Now().GetSeconds(); // Get time in seconds          
          // Format to 1 decimal place
          std::ostringstream oss;
          oss << std::fixed << std::setprecision(1) << nowTime;
          std::string nowTimeStr = oss.str();  // Convert to string
          std::string uniqueId = std::to_string(i) + "_" + nowTimeStr;

          temp_bm->SetBiomarkerID(uniqueId);  // SetBiomarkerID accepts a string
          // temp_bm->SetBiomarkerID(i); // SetBiomarkerID accepts an integer

          temp_bm->SetSize(10); // 10 (µm) in diameter (adjust as needed)
          temp_bm->SetType(biomarkerType);  // Set biomarker type based on infection source
          temp_bm->SetSourceData(sourceData); // Store the source data in the biomarker

          // Install a network device for the biomarker
          temp_bm->InstallBiomarkerNetDevice(biomarkerChannel);
          
          // Get random stream number
          int dr = floor(distributeRandomly->GetValue());
          temp_bm->SetStream(dr);

          // Calculate biomarker position with offset based on group
          temp_bm->SetPosition(Vector(
              m_coordinateStart.x + (directionInterval.x * group),
              m_coordinateStart.y + (directionInterval.y * group),
              m_coordinateStart.z + (directionInterval.z * group)
          ));
          // std::cout << "Released biomarker from InfectionSource into random stream number: " << dr << " biomarker pos: " << temp_bm << std::endl;
          // Add biomarker to the bloodstream
          bloodvessel->AddBiomarkerToStream(dr, temp_bm);
      }
      std::cout << "Finished releasing " << burstIntensity << " biomarkers from the vesselID: " << bloodvessel->GetbloodvesselID() << std::endl;
      // Record time of releasing biomarkers
      double elapsedTime = Simulator::Now().GetSeconds(); // Get time in seconds
      std::cout << "Simulation Time now: " << elapsedTime << " seconds" << std::endl;
      // EDIT .....................    //
      bloodvessel->PrintBiomarkersOfVessel();    
}


double
Bloodcircuit::GetSpeedClassOfBloodVesselType (BloodvesselType type)
{
  if (type == ARTERY)
    {
      return 10.0;
    }
  else if (type == VEIN)
    {
      return 3.7;
    }
  else // if (type == ORGAN)
    {
      return 1.0;
    }
}

void
Bloodcircuit::AddVesselData (int id, BloodvesselType type, Vector start, Vector stop)
{
  Ptr<Bloodvessel> vessel = CreateObject<Bloodvessel> ();
  vessel->SetBloodvesselID (id);
  vessel->SetBloodvesselType (type);
  vessel->SetStartPositionBloodvessel (start);
  vessel->SetStopPositionBloodvessel (stop);
  vessel->SetVesselWidth (0.25); // 0.25
  vessel->SetPrinter (printer);
  vessel->SetPrinterBio (printBio);
  // Init Bloodvessel: Calculate length and angle & velocity.
  vessel->InitBloodstreamLengthAngleAndVelocity (GetSpeedClassOfBloodVesselType (type));
  m_bloodvessels[id] = vessel;
  cout << "New Vessel(" + to_string (id) + "," + to_string (type) + "," + to_string (start.x) +
              "," + to_string (start.y) + "," + to_string (start.z) + "," + to_string (stop.x) +
              "," + to_string (stop.y) + "," + to_string (stop.z) + ")"
       << endl;
}

void
Bloodcircuit::InitialiseBloodvessels (int vesseldata[][8], unsigned int count)
{
  for (unsigned int i = 0; i < count; i++)
    {
      AddVesselData (vesseldata[i][0], (BloodvesselType) vesseldata[i][1],
                     Vector (vesseldata[i][2], vesseldata[i][3], vesseldata[i][4]),
                     Vector (vesseldata[i][5], vesseldata[i][6], vesseldata[i][7]));
    }
}

void
Bloodcircuit::ConnectBloodvessels ()
{
  unsigned int count = m_bloodvessels.size();
  //Set Connections between bloodvessels if they have the same start/end coordinates
  for (unsigned int i = 1; i < count + 1; i++)
    {
      unsigned int counter = 0;
      Vector end = m_bloodvessels[i]->GetStopPositionBloodvessel ();
      for (unsigned int j = 1; j < count + 1; j++)
        {
          Vector start = m_bloodvessels[j]->GetStartPositionBloodvessel ();
          if (end.x == start.x && end.y == start.y && end.z == start.z)
            {
              counter++;
              if (counter == 1)
                {
                  m_bloodvessels[i]->SetNextBloodvessel1 (m_bloodvessels[j]);
                }
              else
                {
                  m_bloodvessels[i]->SetNextBloodvessel2 (m_bloodvessels[j]);
                }
            }
        }
    }
}

} // namespace ns3
