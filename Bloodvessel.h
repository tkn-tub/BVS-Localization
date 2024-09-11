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

#ifndef CLASS_BLOODVESSEL_
#define CLASS_BLOODVESSEL_

#include "PrintBiomarkers.h"
#include "PrintNanobots.h"
#include "Bloodstream.h"
#include <list>
#include "Biomarker.h"
#include "Nanobot.h"

using namespace std;
namespace ns3 {
/**
* \brief Bloodvessel is the place holder of the Nanobot's and manages each step of the Nanobot's mobility.
*
* A Bloodvessel has up to total 5 lists (maximum 5 streams). At each step (interval dt), Bloodvessel
* browses the nanobots of each stream in order of their positions and moves each Nanobot. If the resulting position exceeds the current bloodvessel
* the nanobot gets pushed to the next bloodvessel (and so on).
* Nanobots are added to the Bloodvessels in Bloodcircuit.
*/
enum BloodvesselType { ARTERY, VEIN, ORGAN };
class Bloodvessel : public ns3::Object
{
private:
  bool m_start;
  vector<Ptr<Bloodstream>> m_bloodstreams; // list of nanobots in streams.
  int m_bloodvesselID; // unique ID, set in bloodcircuit.
  double m_bloodvesselLength; // the length of the bloodvessel.
  double m_angle; // the angle of the bloodvessel.
  double m_basevelocity; // velocity of the bloodvessel.
  BloodvesselType m_bloodvesselType; // type of the bloodvessel: 0=artery, 1=vein, 2=organ
  Vector m_startPositionBloodvessel; // start-coordinates of the bloodvessel
  Vector m_stopPositionBloodvessel; // end-coordinates of the bloodvessel
  double m_deltaT; // the mobility step interval (duration between each step).
  double m_vesselWidth; // the width of each stream in the bloodvessel.
  Ptr<PrintNanobots> printer;
  Ptr<PrintBiomarkers> printBio; // EDIT

  /// Connections
  Ptr<Bloodvessel> m_nextBloodvessel1;
  Ptr<Bloodvessel> m_nextBloodvessel2;

  static std::map<Biomarker*, std::list<int>> nanobotsPerBiomarker;  // For global map

  /// Stream settings
  int m_numberOfStreams; // number of streams, maximum value is 5.

  bool m_changeStreamSet; // true, if the nanobots should be able to change their streams.
  int m_currentStream; // current stream number.

  /// Output file with positions and timesteps.
  ofstream m_nbTrace;
  string m_nbTraceFilename;

  const int stream_definition[21][3] = {
      {100, 0, 0},  {95, -1, 0},  {95, +1, 0},  {95, 0, -1},  {95, 0, +1},  {95, -1, -1},
      {95, +1, +1}, {95, +1, -1}, {95, -1, +1}, {90, +2, 0},  {90, -2, 0},  {90, 0, +2},
      {90, 0, -2},  {90, +2, -1}, {90, -2, +1}, {90, -1, +2}, {90, +1, -2}, {90, +2, +1},
      {90, -2, -1}, {90, +1, +2}, {90, -1, -2}};
  const int stream_definition_size = 21;

  Vector SetPosition (Vector nbv, double distance, double angle, int bloodvesselType,
                      double startPosZ);

  // calculate Angle
  double CalcAngle ();

  // calculate Length
  double CalcLength ();

  /**
  * \param value the Number of Streams the bloodvessel can have.
  */
  void initStreams ();

  /// Translates the Nanobots to the new position. Calles TranslatePosition (), ChangeStream (), TransposeNanobots ().
  void TranslateNanobots ();

  /// Calculates the position and velocity of each nanobot for the passed step and the next step.
  void TranslatePosition (double dt);

  /// Changes the nanobot streams if possible. Calles DoChangeStreamIfPossible ().
  void ChangeStream ();

  /// Changes the nanobot streams from current streams to the destination stream.
  void DoChangeStreamIfPossible (int curStream, int desStream);

  /// Transposes Nanobots from one bloodvessel to another.
  void TransposeNanobots (list<Ptr<Nanobot>> reachedEnd, int i);

  /// Moves one Nanobot to the next bloodvessel
  bool transposeNanobot (Ptr<Nanobot> botToTranspose, Ptr<Bloodvessel> thisBloodvessel,
                         Ptr<Bloodvessel> nextBloodvessel, int stream);
  /**
   * /return TRUE if position exceeds bloodvessel
   */
  bool moveNanobot (Ptr<Nanobot> nb, int i, int randVelocityOffset, bool direction, double dt);

  // EDIT .......................... Functions for biomarker movement

    /**
    * \brief Translates the positions of all biomarkers within the blood vessel.
    */
  void TranslateBiomarkers();

  void TranslateBiomarkerPosition(double dt);

    /**
    * \brief Changes the biomarker streams if possible. Calls DoChangeBiomarkerStreamIfPossible ().
    */
    void ChangeBiomarkerStream (); // Add declaration

    /**
     * \brief Changes the biomarker streams from current streams to the destination stream.
     * 
     * \param curStream The current stream of the biomarker.
     * \param desStream The destination stream for the biomarker.
     */
    void DoChangeBiomarkerStreamIfPossible (int curStream, int desStream); // Add declaration


  /**
  * \brief Transposes biomarkers that have reached the end of this blood vessel to the next connected vessel.
  *
  * \param reachedEnd A list of biomarkers that have reached the end of this blood vessel.
  * \param stream The stream ID the biomarkers were in.
  */
   void TransposeBiomarkers(list<Ptr<Biomarker>> reachedEnd, int stream);
 
   bool transposeBiomarker(Ptr<Biomarker> biomarkerToTranspose, Ptr<Bloodvessel> thisBloodvessel, Ptr<Bloodvessel> nextBloodvessel, int stream);

   bool moveBiomarker(Ptr<Biomarker> bm, int i, int randVelocityOffset, bool direction, double dt);

    // Helper function
    bool AreBiomarkersEmpty(); 
    // Remove Biomarker from this vessel
    void RemoveBiomarker(Ptr<Biomarker> biomarker); 


  //GETTER AND SETTER

  /**
  * \returns true, if all streams of the bloodvessel are empty.
  */
  bool IsEmpty ();

  /**
  * \returns the Type of the Bloodvessel.
  */
  BloodvesselType GetBloodvesselType ();

  /**
  * \returns the Angle of the Bloodvessel.
  */
  double GetBloodvesselAngle ();

  /**
  * \returns the Length of the Bloodvessel.
  */
  double GetbloodvesselLength ();

  bool getRandomBoolean ();

public:
  /**
  * Setting the default values:
  * dt=1.0, number of streams=3, changing stream set to true, velocity and current stream is zero.
  * x-, y- and z-direction are empty.
  */
  Bloodvessel ();

  /// Destructor to clean up the lists.
  ~Bloodvessel ();

  /**
  * Starts the bloodvessel.
  */
  void Start ();

  /**
  * Runs one mobility Step for the given bloodvessel.
  * This function is called each interval dt to simulate the mobility through TranslateNanobots ().
  */
  static void Step (Ptr<Bloodvessel> bloodvessel);

  /// Prints all nanobots in the Bloodvessel to a csv file.
  void PrintNanobotsOfVessel ();

  /**
  * \param value the traffic velocity m/s at entrance.
  */
  void InitBloodstreamLengthAngleAndVelocity (double velocity);

  /**
  * \param streamID: ID of Stream
  * \param bot: Pointer to bot to add 
  */
  void AddNanobotToStream (unsigned int streamID, Ptr<Nanobot> bot);

  //GETTER AND SETTER

  /**
  * overrides TypeId.
  */
  static TypeId GetTypeId (void);

  /**
  * \param Id of a Stream 
  * \returns a specific stream
  */
  Ptr<Bloodstream> GetStream (int id);

  /**
  * \returns the ID of the Bloodvessel.
  */
  int GetbloodvesselID ();

  /**
  * \returns the Number of Streams in the Bloodvessel.
  */
  int GetNumberOfStreams ();

  /**
  * \returns the Startposition of the Bloodvessel.
  */
  Vector GetStartPositionBloodvessel ();

  /**
  * \returns the stopposition of the Bloodvessel.
  */
  Vector GetStopPositionBloodvessel ();

  /**
   * gets an Random Object
   */
  Ptr<UniformRandomVariable> getRandomObjectBetween (double min, double max);

  /**
  * \param value the ID of the Bloodvessel.
  */
  void SetBloodvesselID (int b_id);

  /**
  * \param value the type of the Bloodvessel.
  */
  void SetBloodvesselType (BloodvesselType value);

  /**
  * \param value the Width of the Streams.
  */
  void SetVesselWidth (double value);

  /**
  * \param value the start position of the Bloodvessel.
  */
  void SetStartPositionBloodvessel (Vector value);

  /**
  * \param value the start position of the Bloodvessel.
  */
  void SetStopPositionBloodvessel (Vector value);

  /**
  * \param value the following Bloodvessel.
  */
  void SetNextBloodvessel1 (Ptr<Bloodvessel> value);

  /**
  * \param value the following Bloodvessel.
  */
  void SetNextBloodvessel2 (Ptr<Bloodvessel> value);

  void SetPrinter (Ptr<PrintNanobots> printer);

  // EDIT ..................
  /**
  * \brief Sets the PrintBiomarkers object.
  * \param printBio Pointer to a PrintBiomarkers object
  */
  void SetPrinterBio(Ptr<PrintBiomarkers> printBio);

      /**
     * \brief Simulation step for updating biomarker positions.
     * 
     * \param bloodvessel A pointer to the Bloodvessel object.
     */
    static void StepBiomarkers(Ptr<Bloodvessel> bloodvessel);

  /**
  * \brief Calculates the distance between two 3D vectors (points).
  * 
  * \param pos1 The first position vector.
  * \param pos2 The second position vector.
  * \return The Euclidean distance between the two points.
  */
  double CalculateDistance(Vector pos1, Vector pos2);

  /**
  * \brief Checks if any nanobots are within communication range of a given biomarker.
  * 
  * \param biomarker The biomarker to check against.
  * \param communicationThreshold The distance threshold for communication.
  * \return A list of IDs of nanobots within the communication range.
  */
  std::list<int> IsNanobotInRange(Ptr<Biomarker> biomarker, double communicationThreshold);

  /**
  * \brief Prints information about all biomarkers in the blood vessel to a file.
  */
  void PrintBiomarkersOfVessel();

    /**
  * \brief Adds a Biomarker object to a specific stream within the Bloodvessel.
  *
  * \param streamID The ID of the stream where the biomarker will be added.
  * \param biomarker A pointer to the Biomarker object to be added.
  */
  void AddBiomarkerToStream(int streamID, Ptr<Biomarker> biomarker); 

};

}; // namespace ns3
#endif
