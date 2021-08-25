#include <iostream>
#include <libgen.h>     
#include <cstdlib>

#include "AraFilterTools.h"
#include "UsefulAtriStationEvent.h"

//User's includes


//User must set the filter number - this identifies your filter
/*
 ARA_FILTER_RANDOM 
 ARA_FILTER_MIN_BIAS
 ARA_FILTER_TIME_SEQUENCE 
 ARA_FILTER_NCHNL_FILTER 3
 ARA_FILTER_TRACK_ENGINE 4
 ARA_FILTER_UNDEFINED_5 5
 ARA_FILTER_UNDEFINED_6 6
 ARA_FILTER_UNDEFINED_7 7
 ARA_FILTER_UNDEFINED_8 8
 ARA_FILTER_UNDEFINED_9 9
 
 
 THIS_FILTER should be set as follows:
 #define THIS_FILTER ARA_FILTER_RANDOM - Random Filter
 #define THIS_FILTER ARA_FILTER_MIN_BIAS - Min-Bias Filter
 #define THIS_FILTER ARA_FILTER_TIME_SEQUENCE - Time Sequence Filter
 #define THIS_FILTER ARA_FILTER_NCHNL_FILTER - NChnl Filter
 #define THIS_FILTER ARA_FILTER_TRACK_ENGINE - Track Engine Filter
*/
#define THIS_FILTER ARA_FILTER_UNDEFINED_6

using namespace std;

int main(int argc, char **argv) {
  if(argc<7) {
    std::cout << "Usage: " << basename(argv[0]) << " <fileListName> <outDir> <runNumber> <monitoringFile> <numberOfEvents> <pedFile>" << std::endl;
    return -1;
  }
  Int_t runNumber=atoi(argv[3]);
  Int_t numEvents = atoi(argv[5]);
  runFilter(argv[1],argv[2], runNumber, argv[4], numEvents, argv[6], THIS_FILTER);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//  This is where the users code goes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//  You must have the following functions:
//
//    Double_t getQualityParameter(UsefulAtriStationEvent *evPtr);
//    
////////////////////////////////////////////////////////////////////////////////

Double_t getQualityParameter(UsefulAtriStationEvent *evPtr){
  //Do something clever
  Double_t evNum = Double_t(evPtr->eventNumber);
  return -1*evNum;
}
