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

#include "Bloodvessel.h"
#include "Constants.h"
#include <random>
#include <functional>
#include "ns3/log.h" // Include for logging
#include "PrintBiomarkers.h"
#include <map>

using namespace std;
namespace ns3 {

NS_LOG_COMPONENT_DEFINE("Bloodvessel");

auto randomIntegerBetweenZeroAndOne =
    std::bind (std::uniform_int_distribution<> (0, 1), std::default_random_engine ());

TypeId
Bloodvessel::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::Bloodvessel").SetParent<Object> ().AddConstructor<Bloodvessel> ();
  return tid;
}

Bloodvessel::Bloodvessel ()
{
  m_deltaT = 1;
  initStreams ();
  m_changeStreamSet = true;
  m_basevelocity = 0;
  m_currentStream = 0;
  printBio = nullptr; // Initialize the printBio pointer to null

}

Bloodvessel::~Bloodvessel ()
{
  /*  for (int i = 0; i < m_numberOfStreams; i++)
    {
      m_nanobots[i].erase (m_nanobots[i].begin (), m_nanobots[i].end ());
    }
    */
}

void
Bloodvessel::SetPrinter (Ptr<PrintNanobots> printer)
{
  this->printer = printer;
}

// EDIT ................
void Bloodvessel::SetPrinterBio(Ptr<PrintBiomarkers> printBio) {
  this->printBio = printBio;
}

void
Bloodvessel::Start ()
{
  m_start = true;
  Simulator::Schedule (Seconds (0.0), &Step, Ptr<Bloodvessel> (this));
}

void
Bloodvessel::Step (Ptr<Bloodvessel> bloodvessel)
{
  if (bloodvessel->GetbloodvesselID () == 1)
    {
      static clock_t now, old = clock ();
      old = now;
      now = clock ();
      // cout << (now - old) / 1000000 << " -> " << Simulator::Now () << endl;

      // double elapsedTime = Simulator::Now().GetSeconds(); // Get time in seconds
      // std::cout << "calling Translate...: " << elapsedTime << " seconds" << std::endl;
    }
  bloodvessel->TranslateNanobots ();
  bloodvessel->TranslateBiomarkers();
  // cout << "TranslateBiomarkers in step ///.....after...../// TranslateNanobots " <<"\n";
}

Ptr<UniformRandomVariable>
Bloodvessel::getRandomObjectBetween (double min, double max)
{
  Ptr<UniformRandomVariable> random = CreateObject<UniformRandomVariable> ();
  random->SetAttribute ("Min", DoubleValue (min));
  random->SetAttribute ("Max", DoubleValue (max));
  return random;
}

bool
Bloodvessel::getRandomBoolean ()
{
  return randomIntegerBetweenZeroAndOne ();
}

Vector
Bloodvessel::SetPosition (Vector nbv, double distance, double angle, int bloodvesselType,
                          double startPosZ)
{
  //Check vessel direction and move according to distance.
  //right
  if (angle == 0.00 && bloodvesselType != ORGAN)
    {
      nbv.x += distance;
    }
  //left
  else if (angle == -180.00 || angle == 180.00)
    {
      nbv.x -= distance;
    }
  //down
  else if (angle == -90.00)
    {
      nbv.y -= distance;
    }
  //up
  else if (angle == 90.00)
    {
      nbv.y += distance;
    }
  //back
  else if (angle == 0.00 && bloodvesselType == ORGAN && startPosZ == 2)
    {
      nbv.z -= distance;
    }
  //front
  else if (angle == 0.00 && bloodvesselType == ORGAN && startPosZ == -2)
    {
      nbv.z += distance;
    }
  //right up
  else if ((0.00 < angle && angle < 90.00) || (-90.00 < angle && angle < 0.00) ||
           (90.00 < angle && angle < 180.00) || (-180.00 < angle && angle < -90.00))
    {
      nbv.x += distance * (cos (fmod ((angle), 360) * M_PI / 180));
      nbv.y += distance * (sin (fmod ((angle), 360) * M_PI / 180));
    }
  return nbv;
}

void
Bloodvessel::TranslateNanobots ()
{
  if (m_start)
    {
      Simulator::Schedule (Seconds (m_deltaT), &Bloodvessel::Step, Ptr<Bloodvessel> (this));
      m_start = false;
      return;
    }
  if (this->IsEmpty ())
    {
      Simulator::Schedule (Seconds (m_deltaT), &Bloodvessel::Step, Ptr<Bloodvessel> (this));
      return;
    }
  static int loop = 1;
  if (loop == 2)
    {
      loop = 0;
    }
  //Change streams only in organs
  if (loop == 0 && m_changeStreamSet == true && this->GetBloodvesselType () == ORGAN)
    {
      ChangeStream ();
    }
  //Translate their position every timestep
  TranslatePosition (m_deltaT);
  loop++;
  Simulator::Schedule (Seconds (m_deltaT), &Bloodvessel::Step, Ptr<Bloodvessel> (this));
}

bool
Bloodvessel::moveNanobot (Ptr<Nanobot> nb, int i, int randVelocityOffset, bool direction, double dt)
{
  double distance = 0.0;
  double velocity = m_bloodstreams[i]->GetVelocity ();
  if (direction)
    {
      distance = (velocity - ((velocity / 100) * randVelocityOffset)) * dt;
    }
  else
    {
      distance = (velocity + ((velocity / 100) * randVelocityOffset)) * dt;
    }
  //Check vessel direction and move accordingly.
  Vector nbv = SetPosition (nb->GetPosition (), distance, GetBloodvesselAngle (),
                            GetBloodvesselType (), m_startPositionBloodvessel.z);
  nb->SetPosition (nbv);
  nb->SetTimeStep ();
  double nbx = nb->GetPosition ().x - m_startPositionBloodvessel.x;
  double nby = nb->GetPosition ().y - m_startPositionBloodvessel.y;
  double length = sqrt (nbx * nbx + nby * nby);
  //check if position exceeds bloodvessel
  return (length > m_bloodvesselLength || (nbv.z < -2 && m_angle == 0) ||
          (nbv.z > 2 && m_angle == 0));
}

void
Bloodvessel::TranslatePosition (double dt)
{
  list<Ptr<Nanobot>> print;
  list<Ptr<Nanobot>> reachedEnd;

  auto randomVelocityOffset =
      std::bind (std::uniform_int_distribution<> (0, 11), std::default_random_engine ());
  // for every stream of the vessel
  for (int i = 0; i < m_numberOfStreams; i++)
    {
      // for every nanobot of the stream
      for (uint j = 0; j < m_bloodstreams[i]->CountNanobots (); j++)
        {
          Ptr<Nanobot> nb = m_bloodstreams[i]->GetNanobot (j);
          //move only nanobots that have not already been translated by another vessel
          if (nb->GetTimeStep () < Simulator::Now ())
            {
              // has nanobot reached end after moving
              if (moveNanobot (nb, i, randomVelocityOffset (), getRandomBoolean (), dt))
                {
                  reachedEnd.push_back (nb);
                }
              else
                {
                  print.push_back (nb);
                  //PrintNanobot (nb, this);
                }
            } //if timestep not from old
        } //inner for
      //Push Nanobots that reached the end of the bloodvessel to next vessel.
      if (reachedEnd.size () > 0)
        {
          TransposeNanobots (reachedEnd, i);
        }
      reachedEnd.clear ();
    } //outer for
  printer->PrintSomeNanobots (print, this->GetbloodvesselID ());
} //Function

void
Bloodvessel::ChangeStream ()
{
  if (m_numberOfStreams > 1)
    {
      Ptr<UniformRandomVariable> nb_randomize = getRandomObjectBetween (0, m_numberOfStreams);
      //set half of the nanobots randomly to change
      for (int i = 0; i < m_numberOfStreams; i++)
        {
          for (uint j = 0; j < m_bloodstreams[i]->CountNanobots (); j++) //in every stream
            {
              if (getRandomBoolean ())
                { // 50 / 50 Chance for every nanobot to change
                  m_bloodstreams[i]->GetNanobot (j)->SetShouldChange (true);
                }
            }
        }
      // after all nanobots that should change are flagged, do change
      for (int i = 0; i < m_numberOfStreams; i++)
        {
          // Random Number that is "1" or "-1"
          int direction = getRandomBoolean () == true ? -1 : 1;
          if (i == 0)
            { //Special Case 1: outer lane left -> go to middle
              direction = 1;
            }
          else if (i + 1 >= m_numberOfStreams)
            { //Special Case 2: outer lane right -> go to middle
              direction = -1;
            }
          // Move randomly left or right
          DoChangeStreamIfPossible (i, i + direction);
        }
    }
}

void
Bloodvessel::DoChangeStreamIfPossible (int curStream, int desStream)
{
  list<Ptr<Nanobot>> canChange;
  canChange.clear ();
  for (uint j = 0; j < m_bloodstreams[curStream]->CountNanobots (); j++)
    {
      if (m_bloodstreams[curStream]->GetNanobot (j)->GetShouldChange ())
        {
          //set should change back to false
          m_bloodstreams[curStream]->GetNanobot (j)->SetShouldChange (false);
          m_bloodstreams[desStream]->AddNanobot (m_bloodstreams[curStream]->RemoveNanobot (j));
        }
    }
  //Sort all Nanobots by ID
  m_bloodstreams[desStream]->SortStream ();
}

bool
Bloodvessel::transposeNanobot (Ptr<Nanobot> botToTranspose, Ptr<Bloodvessel> thisBloodvessel,
                               Ptr<Bloodvessel> nextBloodvessel, int stream)
{
  Vector stopPositionOfVessel = thisBloodvessel->GetStopPositionBloodvessel ();
  Vector nanobotPosition = botToTranspose->GetPosition ();
  double distance = sqrt (pow (nanobotPosition.x - stopPositionOfVessel.x, 2) +
                          pow (nanobotPosition.y - stopPositionOfVessel.y, 2) +
                          pow (nanobotPosition.z - stopPositionOfVessel.z, 2));
  distance = distance / thisBloodvessel->m_bloodstreams[stream]->GetVelocity () *
             nextBloodvessel->m_bloodstreams[stream]->GetVelocity ();
  botToTranspose->SetPosition (nextBloodvessel->GetStartPositionBloodvessel ());
  Vector rmp = SetPosition (
      botToTranspose->GetPosition (), distance, nextBloodvessel->GetBloodvesselAngle (),
      nextBloodvessel->GetBloodvesselType (), thisBloodvessel->GetStopPositionBloodvessel ().z);
  botToTranspose->SetPosition (rmp);
  double nbx = botToTranspose->GetPosition ().x - nextBloodvessel->GetStartPositionBloodvessel ().x;
  double nby = botToTranspose->GetPosition ().y - nextBloodvessel->GetStartPositionBloodvessel ().y;
  double length = sqrt (nbx * nbx + nby * nby);
  //check if position exceeds bloodvessel
  return length > nextBloodvessel->GetbloodvesselLength () || rmp.z < -2 || rmp.z > 2;
}

void
Bloodvessel::TransposeNanobots (list<Ptr<Nanobot>> reachedEnd, int stream)
{
  list<Ptr<Nanobot>> print1;
  list<Ptr<Nanobot>> print2;
  list<Ptr<Nanobot>> reachedEndAgain;
  Ptr<UniformRandomVariable> rv = getRandomObjectBetween (0, 4);
  int onetwo;
  for (const Ptr<Nanobot> &botToTranspose : reachedEnd)
    {
      onetwo = floor (rv->GetValue ());
      // Remove Nanobot from actual bloodstream
      m_bloodstreams[stream]->RemoveNanobot (botToTranspose);
      //Bias for Vessels that deliver blood to organs - 75% go directly forward in the main vessel - hard coded
      if (m_nextBloodvessel2 != nullptr && (m_nextBloodvessel2->GetbloodvesselID () == 6 ||
                                      m_nextBloodvessel2->GetbloodvesselID () == 32 ||
                                      m_nextBloodvessel2->GetbloodvesselID () == 35))
        {
          if (onetwo == 1)
            {
              onetwo = 2;
            }
        }
      // TODO CD:  Umbauen auf m_nextBloodVessel->getNextBloodvessel->isOrgan()?
      if (m_nextBloodvessel1->GetbloodvesselID () == 3 ||
          m_nextBloodvessel1->GetbloodvesselID () == 7 ||
          m_nextBloodvessel1->GetbloodvesselID () == 12 ||
          m_nextBloodvessel1->GetbloodvesselID () == 24 ||
          m_nextBloodvessel1->GetbloodvesselID () == 25 ||
          m_nextBloodvessel1->GetbloodvesselID () == 37 ||
          m_nextBloodvessel1->GetbloodvesselID () == 43 ||
          m_nextBloodvessel1->GetbloodvesselID () == 45)
        {
          if (onetwo == 2)
            {
              onetwo = 1;
            }
        }
      if (m_nextBloodvessel2 != nullptr && (onetwo == 0 || onetwo == 1))
        {
          if (transposeNanobot (botToTranspose, this, m_nextBloodvessel2, stream))
            {
              reachedEndAgain.push_back (botToTranspose);
              m_nextBloodvessel2->TransposeNanobots (reachedEndAgain, stream);
              reachedEndAgain.clear ();
            }
          else
            {
              m_nextBloodvessel2->m_bloodstreams[stream]->AddNanobot (botToTranspose);
              print2.push_back (botToTranspose);
            }
        }
      else if ((onetwo == 2 || onetwo == 3) || m_nextBloodvessel2 == nullptr)
        {
          if (transposeNanobot (botToTranspose, this, m_nextBloodvessel1, stream))
            {
              reachedEndAgain.push_back (botToTranspose);
              m_nextBloodvessel1->TransposeNanobots (reachedEndAgain, stream);
              reachedEndAgain.clear ();
            }
          else
            {
              m_nextBloodvessel1->m_bloodstreams[stream]->AddNanobot (botToTranspose);
              print1.push_back (botToTranspose);
            }
        }
    } // for
  printer->PrintSomeNanobots (print1, m_nextBloodvessel1->GetbloodvesselID ());
  if (m_nextBloodvessel2)
    {
      printer->PrintSomeNanobots (print2, m_nextBloodvessel2->GetbloodvesselID ());
    }
}

//HELPER
void
Bloodvessel::PrintNanobotsOfVessel ()
{
  for (uint j = 0; j < m_bloodstreams.size (); j++)
    {
      for (uint i = 0; i < m_bloodstreams[j]->CountNanobots (); i++)
        {
          printer->PrintNanobot (m_bloodstreams[j]->GetNanobot (i), GetbloodvesselID ());
        }
    }
}

void
Bloodvessel::initStreams ()
{
  int i;
  Ptr<Bloodstream> stream;
  for (i = 0; i < stream_definition_size; i++)
    {
      stream = CreateObject<Bloodstream> ();
      stream->initBloodstream (m_bloodvesselID, i, stream_definition[i][0],
                               stream_definition[i][1] / 10.0, stream_definition[i][2] / 10.0,
                               GetBloodvesselAngle ());
      m_bloodstreams.push_back (stream);
    }
  m_numberOfStreams = stream_definition_size;
}

double
Bloodvessel::CalcLength ()
{
  if (GetBloodvesselType () == ORGAN)
    {
      return 4;
    }
  else
    {
      Vector m_start = GetStartPositionBloodvessel ();
      Vector m_end = GetStopPositionBloodvessel ();
      double l = sqrt (pow (m_end.x - m_start.x, 2) + pow (m_end.y - m_start.y, 2));
      return l;
    }
}

double
Bloodvessel::CalcAngle ()
{
  Vector m_start = GetStartPositionBloodvessel ();
  Vector m_end = GetStopPositionBloodvessel ();
  double x = m_end.x - m_start.x;
  double y = m_end.y - m_start.y;
  return atan2 (y, x) * 180 / M_PI;
}

void
Bloodvessel::InitBloodstreamLengthAngleAndVelocity (double velocity)
{
  int i;
  double length = CalcLength ();
  m_bloodvesselLength = length < 0 ? 10000 : length;
  m_angle = CalcAngle ();
  if (velocity >= 0)
    {
      m_basevelocity = velocity;
      int maxLength = 0;
      // calulate the width per stream
      for (i = 0; i < m_numberOfStreams; i++)
        {
          if (maxLength < stream_definition[i][1])
            {
              maxLength = stream_definition[i][1];
            }
          if (maxLength < stream_definition[i][2])
            {
              maxLength = stream_definition[i][2];
            }
        }
      double offset = m_vesselWidth / 2.0 / maxLength;
      // Set velocity, angle and position offset
      for (i = 0; i < m_numberOfStreams; i++)
        {
          m_bloodstreams[i]->SetVelocity (m_basevelocity);
          m_bloodstreams[i]->SetAngle (m_angle, stream_definition[i][1] * offset,
                                       stream_definition[i][2] * offset);
        }
    }
}

//GETTER AND SETTER

bool
Bloodvessel::IsEmpty ()
{
  bool empty = true;
  for (int i = 0; i < m_numberOfStreams; i++)
    {
      empty = empty && m_bloodstreams[i]->IsEmpty ();
    }
  return empty;
}

int
Bloodvessel::GetbloodvesselID ()
{
  return m_bloodvesselID;
}

void
Bloodvessel::SetBloodvesselID (int b_id)
{
  m_bloodvesselID = b_id;
}

double
Bloodvessel::GetBloodvesselAngle ()
{
  return m_angle;
}

int
Bloodvessel::GetNumberOfStreams ()
{
  return m_numberOfStreams;
}

Ptr<Bloodstream>
Bloodvessel::GetStream (int id)
{
  return m_bloodstreams[id];
}

double
Bloodvessel::GetbloodvesselLength ()
{
  return m_bloodvesselLength;
}

void
Bloodvessel::SetVesselWidth (double value)
{
  m_vesselWidth = value;
}

void
Bloodvessel::AddNanobotToStream (unsigned int streamID, Ptr<Nanobot> bot)
{
  m_bloodstreams[streamID]->AddNanobot (bot);
}


BloodvesselType
Bloodvessel::GetBloodvesselType ()
{
  return m_bloodvesselType;
}

void
Bloodvessel::SetBloodvesselType (BloodvesselType value)
{
  m_bloodvesselType = value;
}

void
Bloodvessel::SetNextBloodvessel1 (Ptr<Bloodvessel> value)
{
  m_nextBloodvessel1 = value;
}

void
Bloodvessel::SetNextBloodvessel2 (Ptr<Bloodvessel> value)
{
  m_nextBloodvessel2 = value;
}

Vector
Bloodvessel::GetStartPositionBloodvessel ()
{
  return m_startPositionBloodvessel;
}

void
Bloodvessel::SetStartPositionBloodvessel (Vector value)
{
  m_startPositionBloodvessel = value;
}

Vector
Bloodvessel::GetStopPositionBloodvessel ()
{
  return m_stopPositionBloodvessel;
}

void
Bloodvessel::SetStopPositionBloodvessel (Vector value)
{
  m_stopPositionBloodvessel = value;
}

// EDIT.................... MOVEMENT ...............................
void Bloodvessel::TranslateBiomarkers() 
{
    // cout << "In Bloodvessel::TranslateBiomarkers " <<"\n";
    if (m_start) {
        Simulator::Schedule(Seconds(m_deltaT), &Bloodvessel::StepBiomarkers, Ptr<Bloodvessel>(this));
        m_start = false;
        return;
    }
    
    if (this->AreBiomarkersEmpty()) { 
        Simulator::Schedule(Seconds(m_deltaT), &Bloodvessel::StepBiomarkers, Ptr<Bloodvessel>(this));
        return;
    }
    static int loop = 1;
    if (loop == 2)
      {
        loop = 0;
      }
    //Change streams only in organs
    if (loop == 0 && m_changeStreamSet == true && this->GetBloodvesselType () == ORGAN)
      {
        ChangeBiomarkerStream ();
      }
    //Translate their position every timestep
    TranslateBiomarkerPosition(m_deltaT);
    loop++;
    Simulator::Schedule(Seconds(m_deltaT), &Bloodvessel::StepBiomarkers, Ptr<Bloodvessel>(this));
    
}

void Bloodvessel::StepBiomarkers(Ptr<Bloodvessel> bloodvessel) {
    bloodvessel->TranslateBiomarkers();
    // cout << "in step biomarkers " <<"\n";
}

bool Bloodvessel::AreBiomarkersEmpty(){
  // cout << "In Bloodvessel::AreBiomarkersEmpty" << "\n";
	bool empty = true;
  for (int i = 0; i < m_numberOfStreams; i++) {
    empty = empty && m_bloodstreams[i]->AreBiomarkersEmpty();
    // cout << "inside for loop in Bloodvessel::AreBiomarkersEmpty" << "\n";
  }
 return empty;
}

void Bloodvessel::TranslateBiomarkerPosition(double dt) 
{
    // cout << "In Bloodvessel::TranslateBiomarkerPosition" << "\n";
    std::list<Ptr<Biomarker>> print;       // Biomarkers whose positions need to be printed
    std::list<Ptr<Biomarker>> reachedEnd;   // Biomarkers that reached the end of the vessel
    std::list<Ptr<Biomarker>> biomarkersToRemove; // Track biomarkers to remove
    
  auto randomVelocityOffset = std::bind (std::uniform_int_distribution<> (0, 11), std::default_random_engine ());
  // for every stream of the vessel     // Iterate through bloodstreams and biomarkers
    for (int i = 0; i < m_numberOfStreams; i++) {
      // for every biomarker of the stream
      for (uint j = 0; j < m_bloodstreams[i]->CountBiomarkers (); j++)
      {
            Ptr<Biomarker> bm = m_bloodstreams[i]->GetBiomarker (j);
            //move only Biomarkers that have not already been translated by another vessel
            if (bm->GetTimeStep() < Simulator::Now()) {
                // Check for expired biomarkers
                if (bm->ShouldBeRemoved()) {
                    //it = m_bloodstreams[i]->EraseBiomarker(it); // Remove and get next iterator
                    biomarkersToRemove.push_back(bm); 
                } else {
                    // Move the biomarker if not expired // has biomarker reached end after moving
                    if (moveBiomarker(bm, i, randomVelocityOffset (), getRandomBoolean(), dt)) { 
                        reachedEnd.push_back(bm); 
                    } else {
                        print.push_back(bm);
                    }
                    //++it; // Increment the iterator only if the biomarker wasn't removed
                }
            } 
        }
        //Push Biomarkers that reached the end of the bloodvessel to next vessel.
        if (reachedEnd.size() > 0) {
            TransposeBiomarkers(reachedEnd, i);
        }
        reachedEnd.clear();
    }
    printBio->PrintSomeBiomarkers (print, this->GetbloodvesselID ());
    // cout << "in TRANSLSTE biomarkers positionb " <<"\n";
}

// Implement ChangeBiomarkerStream (similar to ChangeStream for nanobots)
void Bloodvessel::ChangeBiomarkerStream() {
  // cout << "In Bloodvessel::ChangeBiomarkerStream" << "\n";
  if (m_numberOfStreams > 1) {
    Ptr<UniformRandomVariable> bmRandomize = getRandomObjectBetween(0, m_numberOfStreams);

    // Randomly select half of the biomarkers to change streams
    for (int i = 0; i < m_numberOfStreams; i++) {
      for (uint j = 0; j < m_bloodstreams[i]->CountBiomarkers(); j++) {
        if (getRandomBoolean()) {
          m_bloodstreams[i]->GetBiomarker(j)->SetShouldChange(true);
        }
      }
    }

    // Change streams for biomarkers that are flagged to change
    for (int i = 0; i < m_numberOfStreams; i++) {
      // Special cases for outer streams
      int direction = (getRandomBoolean()) ? -1 : 1;
      if (i == 0) {
        direction = 1;
      } else if (i + 1 >= m_numberOfStreams) {
        direction = -1;
      }
      DoChangeBiomarkerStreamIfPossible(i, i + direction);
    }
  }
}

// Implement DoChangeBiomarkerStreamIfPossible (similar to DoChangeStreamIfPossible for nanobots)
void Bloodvessel::DoChangeBiomarkerStreamIfPossible(int curStream, int desStream) {
  // cout << "In Bloodvessel::DoChangeBiomarkerStreamIfPossible" << "\n";

  // Check if destination stream exists and is valid
  if (desStream < 0 || desStream >= m_numberOfStreams) {
    cout << "In Bloodvessel::DoChangeBiomarkerStreamIfPossible: Invalid destination stream index" << "\n";
    return;
  }

  // Gather biomarkers that should change streams
  for (uint j = 0; j < m_bloodstreams[curStream]->CountBiomarkers(); j++) 
  {
    Ptr<Biomarker> bm = m_bloodstreams[curStream]->GetBiomarker(j);
    if (bm->GetShouldChange()) {
          //set should change back to false
          m_bloodstreams[curStream]->GetBiomarker (j)->SetShouldChange (false);
          m_bloodstreams[desStream]->AddBiomarker (m_bloodstreams[curStream]->RemoveBiomarker (j));
    }
  }
  //Sort all biomarkers by ID
  m_bloodstreams[desStream]->SortStream ();
}


bool Bloodvessel::moveBiomarker(Ptr<Biomarker> bm, int i, int randVelocityOffset, bool direction, double dt) {
  double distance = 0.0;
  double velocity = m_bloodstreams[i]->GetVelocity ();
  if (direction)
    {
      distance = (velocity - ((velocity / 100) * randVelocityOffset)) * dt;
    }
  else
    {
      distance = (velocity + ((velocity / 100) * randVelocityOffset)) * dt;
    }
  //Check vessel direction and move accordingly.
  Vector nbv = SetPosition (bm->GetPosition (), distance, GetBloodvesselAngle (),
                            GetBloodvesselType (), m_startPositionBloodvessel.z);
  bm->SetPosition (nbv);
  bm->SetTimeStep ();
  double bx = bm->GetPosition ().x - m_startPositionBloodvessel.x;
  double by = bm->GetPosition ().y - m_startPositionBloodvessel.y;
  double length = sqrt (bx * bx + by * by);
  //check if position exceeds bloodvessel
  return (length > m_bloodvesselLength || (nbv.z < -2 && m_angle == 0) ||
          (nbv.z > 2 && m_angle == 0));
}

bool Bloodvessel::transposeBiomarker(Ptr<Biomarker> biomarkerToTranspose, Ptr<Bloodvessel> thisBloodvessel, Ptr<Bloodvessel> nextBloodvessel, int stream) 
{
    Vector stopPositionOfVessel = thisBloodvessel->GetStopPositionBloodvessel();
    Vector biomarkerPosition = biomarkerToTranspose->GetPosition();
    double distance = sqrt(pow(biomarkerPosition.x - stopPositionOfVessel.x, 2) +
                           pow(biomarkerPosition.y - stopPositionOfVessel.y, 2) +
                           pow(biomarkerPosition.z - stopPositionOfVessel.z, 2));
    // Adjust distance based on velocity difference between vessels
    distance = distance / thisBloodvessel->m_bloodstreams[stream]->GetVelocity() * nextBloodvessel->m_bloodstreams[stream]->GetVelocity(); 

    biomarkerToTranspose->SetPosition(nextBloodvessel->GetStartPositionBloodvessel());
    Vector rmp = SetPosition(
            biomarkerToTranspose->GetPosition(), distance, nextBloodvessel->GetBloodvesselAngle(),
            nextBloodvessel->GetBloodvesselType(), thisBloodvessel->GetStopPositionBloodvessel().z);
    biomarkerToTranspose->SetPosition(rmp);

    double bx = biomarkerToTranspose->GetPosition().x - nextBloodvessel->GetStartPositionBloodvessel().x;
    double by = biomarkerToTranspose->GetPosition().y - nextBloodvessel->GetStartPositionBloodvessel().y;
    double length = sqrt(bx * bx + by * by);

    // Check if position exceeds bloodvessel
    return length > nextBloodvessel->GetbloodvesselLength() || rmp.z < -2 || rmp.z > 2;
}

void Bloodvessel::TransposeBiomarkers(list<Ptr<Biomarker>> reachedEnd, int stream) {
    // cout << "In Bloodvessel::TransposeBiomarkers" << "\n";
    list<Ptr<Biomarker>> biomarkersToMove1; // Biomarkers moved to m_nextBloodvessel1
    list<Ptr<Biomarker>> biomarkersToMove2; // Biomarkers moved to m_nextBloodvessel2
    list<Ptr<Biomarker>> reachedEndAgain; // Biomarkers that reach the end of the next vessel
    list<Ptr<Biomarker>> toRemove;      // Biomarkers to be removed due to expiration

    Ptr<UniformRandomVariable> rv = getRandomObjectBetween(0, 4);

    for (const Ptr<Biomarker> &biomarkerToTranspose : reachedEnd) {
        int randomChoice = floor(rv->GetValue());

        // Remove Biomarker from actual bloodstream
        m_bloodstreams[stream]->RemoveBiomarker(biomarkerToTranspose);
        
        // Check if biomarker is expired BEFORE attempting to move
        if (biomarkerToTranspose->ShouldBeRemoved()) {
            toRemove.push_back(biomarkerToTranspose); 
            // Print message here if you want to track biomarker expiration:
            cout <<"Biomarker " << biomarkerToTranspose->GetBiomarkerID() << " expired and removed from vessel " << GetbloodvesselID() << "\n";
            continue; // Skip to the next biomarker
        }

        // Bias for Vessels that deliver blood to organs - 75% go directly forward in the main vessel
        // same as done for nanobots
        if (m_nextBloodvessel2 && (m_nextBloodvessel2->GetbloodvesselID() == 6 ||
                                    m_nextBloodvessel2->GetbloodvesselID() == 32 ||
                                    m_nextBloodvessel2->GetbloodvesselID() == 35)) {
            if (randomChoice == 1) {
                randomChoice = 2;
            }
        }
        // Same as done for nanobots | TODO CD:  Replace hardcoded vessel IDs with a more flexible mechanism
        // maybe look into MEHLISSA == m_nextBloodVessel->getNextBloodvessel->isOrgan()?
        if (m_nextBloodvessel1->GetbloodvesselID() == 3 ||
            m_nextBloodvessel1->GetbloodvesselID() == 7 ||
            m_nextBloodvessel1->GetbloodvesselID() == 12 ||
            m_nextBloodvessel1->GetbloodvesselID() == 24 ||
            m_nextBloodvessel1->GetbloodvesselID() == 25 ||
            m_nextBloodvessel1->GetbloodvesselID() == 37 ||
            m_nextBloodvessel1->GetbloodvesselID() == 43 ||
            m_nextBloodvessel1->GetbloodvesselID() == 45) {
            if (randomChoice == 2) {
                randomChoice = 1;
            }
        }
        if (m_nextBloodvessel2 != nullptr && (randomChoice == 0 || randomChoice == 1)) 
        {
            if (transposeBiomarker(biomarkerToTranspose, this, m_nextBloodvessel2, stream)) {
                reachedEndAgain.push_back(biomarkerToTranspose);
                m_nextBloodvessel2->TransposeBiomarkers(reachedEndAgain, stream);
                reachedEndAgain.clear();
            } else {
                m_nextBloodvessel2->m_bloodstreams[stream]->AddBiomarker (biomarkerToTranspose);
                biomarkersToMove2.push_back(biomarkerToTranspose); 
            }
        } else if ((randomChoice == 2 || randomChoice == 3) || m_nextBloodvessel2 == nullptr)
        {
            if (transposeBiomarker(biomarkerToTranspose, this, m_nextBloodvessel1, stream)) {
                reachedEndAgain.push_back(biomarkerToTranspose);
                m_nextBloodvessel1->TransposeBiomarkers(reachedEndAgain, stream);
                reachedEndAgain.clear();
            } else {
                m_nextBloodvessel1->m_bloodstreams[stream]->AddBiomarker (biomarkerToTranspose); //AddBiomarkerToStream(stream, biomarkerToTranspose);
                biomarkersToMove1.push_back(biomarkerToTranspose); 
            }
        }
        else {
            cout << "last else of Bloodvessel::TransposeBiomarkers" << "\n";
        }
    }    
    printBio->PrintSomeBiomarkers(biomarkersToMove1, m_nextBloodvessel1->GetbloodvesselID());
    if (m_nextBloodvessel2) {
        printBio->PrintSomeBiomarkers(biomarkersToMove2, m_nextBloodvessel2->GetbloodvesselID());
    }
    // Print biomarkers that have been expired
    printBio->PrintSomeBiomarkers(toRemove, this->GetbloodvesselID()); 
}

void Bloodvessel::RemoveBiomarker(Ptr<Biomarker> biomarker) {
    int streamID = biomarker->GetStream();
    m_bloodstreams[streamID]->RemoveBiomarker(biomarker); // RemoveBiomarker is in Bloodstream
}

// EDIT ................................. // 

// EDIT ................................. // Implement the CalculateDistance method
double Bloodvessel::CalculateDistance(Vector pos1, Vector pos2) {
    return std::sqrt(
        std::pow(pos1.x - pos2.x, 2) +
        std::pow(pos1.y - pos2.y, 2) +
        std::pow(pos1.z - pos2.z, 2)
    );
}


std::list<int> Bloodvessel::IsNanobotInRange(Ptr<Biomarker> biomarker, double communicationThreshold) {
    NS_LOG_FUNCTION(this << biomarker << communicationThreshold);
    std::list<int> nearbyNanobotIds;
    cout << "In Bloodvessel::IsNanobotInRange" << biomarker << " thres: " << communicationThreshold << "\n";

    // Error Handling: Check if the MobilityModel is correctly associated with the biomarker's Node
    if (!biomarker->GetNode() || !biomarker->GetNode()->GetObject<MobilityModel>()) { //use getter
        cout << "Bloodvessel::IsNanobotInRange: No MobilityModel associated with the biomarker's node." << "\n";
        //return false; // Or handle the error differently
    }

    Vector biomarkerPos = biomarker->GetPosition();   //->GetNode()->GetObject<MobilityModel>()->GetPosition(); // Use GetNode() here

    // Get the stream ID the biomarker is in
    int streamId = biomarker->GetStream();

    // Iterate through all nanobots in that stream to find nearby ones
    for (uint32_t j = 0; j < m_bloodstreams[streamId]->CountNanobots(); j++) 
    {
        Ptr<Nanobot> nanobot = m_bloodstreams[streamId]->GetNanobot(j);
        // Calculate distance
        Vector nanobotPos = nanobot->GetPosition();
        double distance = CalculateDistance(biomarkerPos, nanobotPos);
        // Store ID if in range
        if (distance <= communicationThreshold) {
            nearbyNanobotIds.push_back(nanobot->GetNanobotID()); // Store the nanobot's ID
        }
    }    
    // 2. Update Cache and Return
    //UpdateNanobotsInRange( nearbyNanobotsIds); // Update the map after calculating

    // Return the list of nearby nanobot IDs
    return nearbyNanobotIds; 
}
// ........................................

void Bloodvessel::AddBiomarkerToStream(int streamID, Ptr<Biomarker> biomarker) 
{
    //cout << "In Bloodvessel::AddBiomarkerToStream" << "\n";
    m_bloodstreams[streamID]->AddBiomarker (biomarker);
}

void Bloodvessel::PrintBiomarkersOfVessel() 
{
  //cout << "In Bloodvessel::PrintBiomarkersOfVessel" << "\n";
    // Iterate through each bloodstream
    for (uint j = 0; j < m_bloodstreams.size(); j++) {
        // Iterate through each biomarker in the bloodstream
        for (uint i = 0; i < m_bloodstreams[j]->CountBiomarkers(); i++) 
        {                       
            if (printBio != nullptr)   // Make sure the printer is initialized
            {  
                // cout << "In Bloodvessel::PrintBiomarkersOfVessel Use PrintBiomarker" << "\n";
                // Go to PrintBiomarker in PrintBiomarkers
                printBio->PrintBiomarker (m_bloodstreams[j]->GetBiomarker (i), GetbloodvesselID ());
            } else {
                cout << "Biomarker printer not initialized!" << "\n"; 
            }
        }
    }
}

// ........................................................................

} // namespace ns3