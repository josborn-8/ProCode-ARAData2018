# - Find ARADISPLAY
# Find the native ARADISPLAY includes and library
#
# ARADISPLAY_INCLUDE_DIRS - where to find AraRoot headers
# ARADISPLAY_LIBRARIES - List of libraries when using AraRoot.
# ARADISPLAY_FOUND - True if AraRoot found.


if (ARADISPLAY_INCLUDE_DIRS)
  # Already in cache, be silent
  set (ARADISPLAY_FIND_QUIETLY TRUE)
endif (ARADISPLAY_INCLUDE_DIRS)


#Need to include some hint paths

find_path (ARADISPLAY_INCLUDE_DIRS 

AraAtriCanvasMaker.h        AraCorrelationFactory.h AraDisplayConventions.h AraIcrrCanvasMaker.h
AraControlPanel.h       AraDisplay.h            AraFFTGraph.h           AraWaveformGraph.h

 HINTS $ENV{ARA_UTIL_INSTALL_DIR}/include)

find_library (ARADISPLAY_LIBRARIES NAMES AraDisplay HINTS $ENV{ARA_UTIL_INSTALL_DIR}/lib)

# handle the QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (ARADISPLAY DEFAULT_MSG ARADISPLAY_LIBRARIES
  ARADISPLAY_INCLUDE_DIRS)

mark_as_advanced (ARADISPLAY_LIBRARIES ARADISPLAY_INCLUDE_DIRS)

# create AraRoot target
add_library(AraDisplay UNKNOWN IMPORTED)
set_property(TARGET AraDisplay PROPERTY IMPORTED_LOCATION "${ARADISPLAY_LIBRARIES}")

message(STATUS "ARADISPLAY_LIBRARIES ${ARADISPLAY_LIBRARIES} ARADISPLAY_INCLUDE_DIRS ${ARADISPLAY_INCLUDE_DIRS}")
