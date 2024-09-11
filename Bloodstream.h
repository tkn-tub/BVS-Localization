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
 * Author: Chris Deter <chris.deter@student.uni-luebeck.de>, Regine Geyer <geyer@itm.uni-luebeck.de>
 */

#ifndef CLASS_BLOODSTREAM_
#define CLASS_BLOODSTREAM_

#include "Nanobot.h"
#include "Biomarker.h"

using namespace std;
namespace ns3 {
/**
* \brief Short Description
*
* Long Description
*/
class Bloodstream : public ns3::Object
{
private:
  int m_bloodvesselID; // unique ID, set in bloodcircuit.

  /// Stream settings
  int m_currentStream; // id of stream
  double m_velocity; // velocity of the bloodvessel.
  int m_velocity_factor;
  // bool m_changeStreamSet; // true, if the nanobots should be able to change their streams.
  double m_offset_x;
  double m_offset_y;
  double m_offset_z;
  list<Ptr<Nanobot>> m_nanobots; // List to store bots in the bloodstream
  // EDIT
  list<Ptr<Biomarker>> m_biomarkers; // List to store biomarkers in the bloodstream


public:
  Bloodstream (void);

  /// Destructor.
  ~Bloodstream (void);

  // initialize bloodstream
  /**
   * \param vesselId: Id of the Vessel
   * \param streamID: Id of the Stream 
   * \param velocityfactor: 1 - 100 Factor which defines velocity compared to the base velocity
   * \param offsetX: relative offsetcoordinates
   * \param offsetY: relative offsetcoordinates
   * \param angle: angle of the stream
   */
  void initBloodstream (int vesselId, int streamId, int velocityfactor, double offsetX,
                        double offsetY, double angle);

  /**
   * \return number of nanobots inside of this stream
   */
  size_t CountNanobots (void);

  /**
 * \param index: NanobotID
 */
  Ptr<Nanobot> GetNanobot (int index);

  /**
 * \param index: NanobotID
 */
  Ptr<Nanobot> RemoveNanobot (int index);

  /**
 * \param bot: pointer to bot
 */
  Ptr<Nanobot> RemoveNanobot (Ptr<Nanobot> bot);

  /**
 * \param bot: pointer to bot
 */
  void AddNanobot (Ptr<Nanobot> bot);

 //////////////////////// EDIT ....................................
  /**
 * \param biomarker: pointer to biomarker
 */
  void AddBiomarker(Ptr<Biomarker> biomarker); 

   
  /**
  * \returns the number of biomarkers in the bloodstream.
  */
  size_t CountBiomarkers();
  
  /**
    * \param i Index of the Biomarker to retrieve
    * \return Pointer to Biomarker at the given index
    */
  Ptr<Biomarker> GetBiomarker(int indx);
  
  /**
  * \param index: BiomarkerID
  */
  Ptr<Biomarker> RemoveBiomarker (int indx);

  /**
  * \param biomarker: pointer to biomarker
  */
  Ptr<Biomarker> RemoveBiomarker (Ptr<Biomarker> biomarker);


  /**
    * \param it Iterator to the Biomarker to remove
    * \return Iterator to the next Biomarker
    */
  std::list<Ptr<Biomarker>>::iterator EraseBiomarker(std::list<Ptr<Biomarker>>::iterator it);

  /**
  * \brief Checks if the bloodstream is empty of biomarkers.
  * \return True if the bloodstream is empty, false otherwise.
  */
  bool AreBiomarkersEmpty(); 
  
  ////////////////////////////

  /**
 * Sort the stream
 */
  void SortStream (void);

  /**
 * \return true if empty
 */
  bool IsEmpty (void);

  /**
 * \return velocity
 */
  double GetVelocity (void);

  /**
 * \parameter new velocity
 */
  void SetVelocity (double velocity);

  /**
   * \param offsetX: relative offsetcoordinates
   * \param offsetY: relative offsetcoordinates
   * \param angle: angle of the stream
 */
  void SetAngle (double angle, double offsetX, double offsetY);

}; //  Class End
}; // namespace ns3
#endif
