# BVS-Bio: An ns-3 Extension to Simulate Intra-Body Nano-Communication
Saswati Pal <pal@ccs-labs.org>
v3.1, 2024-09-09

# Introduction

Existing code: https://github.com/RegineWendt/blood-voyager-s
Regine Wendt <wendt@itm.uni-luebeck.de>
v1.1, 2019-04-24

https://github.com/tkn-tub/BVS_Net
Laurenz Ebner <ebnerlaurenz@gmail.com>
v2.1, 2024-02-12

To enhance the realism of existing BloodVoyager Simulator (BVS), BVS-Bio introduces multiple classes to integrate the source of infection, release biomarkers from the infection, and mobility of the biomarkers in BVS. 
The modules also enable communication between biomarkers and on-body gateways through a network device. It further facilitates interactions with nanobots.

The system is implemented in the Bloodcircuit class within the BVS framework. It includes several classes that manage the creation, positioning, communication, and lifecycle of biomarkers released from an infection source.

Key Components
LocateInfectionSource Method: Simulates the release of biomarkers from an infection source within a blood vessel.
Calculates the direction vector of the vessel and generates unique biomarker types based on the vessel's ID.
Triggers initial biomarker release and schedules continuous releases at 1-second intervals during the simulation.

releaseBiomarkers Method: Handles the generation and distribution of biomarkers in the bloodstream.
Assigns unique IDs to biomarkers and places them within the blood vessel using calculated intervals and direction vectors.
Creates a BiomarkerChannel for communication between biomarkers and nanobots.

Biomarker Class: Models biomarker entities, managing their properties, behaviors, and interactions within the BVS.
Includes attributes like ID, size, type, and source data.
Manages the lifecycle, positioning, and network communication of biomarkers using BiomarkerNetDevice.

Bloodstream Class: Manages a collection of biomarkers within the bloodstream.
Provides methods to add, remove, and count biomarkers.
Manages the lifecycle of biomarkers, including their creation and removal based on predefined lifespans.

BiomarkerChannel Class: Manages communication between biomarkers and nanobots within a blood vessel.
Handles the transmission of data packets from biomarkers to nanobots using NanoNetDevice.
Facilitates the detection of nanobots within range and ensures correct data exchange.

BiomarkerNetDevice Class: Implements a network device to enable communication between biomarkers and nanobots.
Associates biomarkers with communication channels and manages data packet transmission.
Verifies biomarker associations and manages network addresses and channels.

PrintBiomarkers Class: Logs biomarkers detected at the gateways positioned at various body locations (heart, left arm, right arm, left hip, right hip).
Tracks biomarkers' positions and stream IDs over time, outputting data to CSV files for analysis.
