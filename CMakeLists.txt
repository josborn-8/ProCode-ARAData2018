cmake_minimum_required (VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(FFTW3_HINT_INCLUDES "/cvmfs/ara.opensciencegrid.org/trunk/centos7/misc_build/include/include")
set(FFTW3_HINT_LIBRARIES "/cvmfs/ara.opensciencegrid.org/trunk/centos7/misc_build/lib")


project(AraRootAnalysis)
find_package(ROOT REQUIRED COMPONENTS MathMore Gui)
find_package(libRootFftwWrapper REQUIRED)
find_package(FFTW REQUIRED)
find_package(sqlite3 REQUIRED)
find_package(zlib REQUIRED)
find_package(AraEvent REQUIRED)
find_package(AraDisplay REQUIRED)

Set(INCLUDE_DIRECTORIES  ${ARAEVENT_INCLUDE_DIRS} ${LIBROOTFFTWWRAPPER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})
include_directories( ${INCLUDE_DIRECTORIES} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Executables


##set(EXEC_1 newFilter)
set(EXEC_2 AraEventPicker)
##set(EXEC_3 timeSequenceFilter)
set(EXEC_4 randomFilter)
##set(EXEC_5 calPulserPicker)
##set(EXEC_6 testFilter2)
##set(EXEC_7 nchnlFilter)
#set(EXEC_8 trackEngineFilter)

##add_executable(${EXEC_1} ${EXEC_1}.cxx fileWriterUtil.c AraFilterTools.cxx)
add_executable(${EXEC_2} ${EXEC_2}.cxx fileWriterUtil.c AraFilterTools.cxx)
##add_executable(${EXEC_3} ${EXEC_3}.cxx fileWriterUtil.c AraFilterTools.cxx)
add_executable(${EXEC_4} ${EXEC_4}.cxx fileWriterUtil.c AraFilterTools.cxx)
##add_executable(${EXEC_5} ${EXEC_5}.cxx fileWriterUtil.c AraFilterTools.cxx)
##add_executable(${EXEC_6} ${EXEC_6}.cxx fileWriterUtil.c AraFilterTools.cxx)
##add_executable(${EXEC_7} ${EXEC_7}.cxx fileWriterUtil.c AraFilterTools.cxx)
#add_executable(${EXEC_8} ${EXEC_8}.cxx fileWriterUtil.c AraFilterTools.cxx)

##target_link_libraries(${EXEC_1}  ${ARAEVENT_LIBRARIES} ${LIBROOTFFTWWRAPPER_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
target_link_libraries(${EXEC_2}  ${ARAEVENT_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
##target_link_libraries(${EXEC_3}  ${ARAEVENT_LIBRARIES} ${LIBROOTFFTWWRAPPER_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
target_link_libraries(${EXEC_4}  ${ARAEVENT_LIBRARIES} ${LIBROOTFFTWWRAPPER_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
##target_link_libraries(${EXEC_5}  ${ARAEVENT_LIBRARIES} ${LIBROOTFFTWWRAPPER_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
##target_link_libraries(${EXEC_6}  ${ARAEVENT_LIBRARIES} ${LIBROOTFFTWWRAPPER_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
##target_link_libraries(${EXEC_7}  ${ARAEVENT_LIBRARIES} ${LIBROOTFFTWWRAPPER_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})
#target_link_libraries(${EXEC_8}  ${ARAEVENT_LIBRARIES} ${LIBROOTFFTWWRAPPER_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})





