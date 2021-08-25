# - Find ARAEVENT
# Find the native ARAEVENT includes and library
#
# ARAEVENT_INCLUDES - where to find AraRoot headers
# ARAEVENT_LIBRARIES - List of libraries when using AraRoot.
# ARAEVENT_FOUND - True if AraRoot found.


if (ARAEVENT_INCLUDES)
  # Already in cache, be silent
  set (ARAEVENT_FIND_QUIETLY TRUE)
endif (ARAEVENT_INCLUDES)


#Need to include some hint paths

find_path (ARAEVENT_INCLUDES 

AraAntennaInfo.h            AraStationInfo.h            RawIcrrStationEvent.h       araIcrrDefines.h
AraEventCalibrator.h        AtriEventHkData.h           RawAraGenericHeader.h       RawIcrrStationHeader.h      araIcrrStructures.h
AtriSensorHkData.h          RawAraStationEvent.h        UsefulAraStationEvent.h     araSoft.h  			AraGeomTool.h
FullIcrrHkEvent.h           RawAtriSimpleStationEvent.h UsefulAtriStationEvent.h    AraRawIcrrRFChannel.h       IcrrHkData.h                
RawAtriStationBlock.h       UsefulIcrrStationEvent.h   	AraRootVersion.h            IcrrTriggerMonitor.h        RawAtriStationEvent.h       
araAtriStructures.h

 HINTS $ENV{ARA_UTIL_INSTALL_DIR}/include)

find_library (ARAEVENT_LIBRARIES NAMES AraEvent HINTS $ENV{ARA_UTIL_INSTALL_DIR}/lib)

# handle the QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (ARAEVENT DEFAULT_MSG ARAEVENT_LIBRARIES
  ARAEVENT_INCLUDES)

mark_as_advanced (ARAEVENT_LIBRARIES ARAEVENT_INCLUDES)

# create AraRoot target
add_library(AraEvent UNKNOWN IMPORTED)
set_property(TARGET AraEvent PROPERTY IMPORTED_LOCATION "${ARAEVENT_LIBRARIES}")

message(STATUS "ARAEVENT_LIBRARIES ${ARAEVENT_LIBRARIES} ARAEVENT_INCLUDES ${ARAEVENT_INCLUDES}")
