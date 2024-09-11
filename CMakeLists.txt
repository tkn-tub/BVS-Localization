build_lib(
  LIBNAME blood-voyager-s
  
  SOURCE_FILES Bloodvessel.cc
    Bloodstream.cc
    Nanobot.cc
    Bloodcircuit.cc
    PrintNanobots.cc
    BVSChannel.cc
    NanoNetDevice.cc
    GatewayNetDevice.cc
    terahertz.cc
    Bloodvessel.cc
    Biomarker.cc
    BiomarkerChannel.cc
    BiomarkerNetDevice.cc
    PrintBiomarkers.cc     
	
   
  HEADER_FILES Bloodvessel.h
    Bloodstream.h
    Nanobot.h
    Bloodcircuit.h
    PrintNanobots.h
    BVSChannel.h
    NanoNetDevice.h
    GatewayNetDevice.h
    terahertz.h
    Bloodvessel.h
    Biomarker.h
    BiomarkerChannel.h
    BiomarkerNetDevice.h
    PrintBiomarkers.h

  
  LIBRARIES_TO_LINK
    ${libinternet}
    ${libcore}
    ${libmobility}
    ${libwifi}
    ${libnetwork}
)