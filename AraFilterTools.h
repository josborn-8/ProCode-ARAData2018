#ifndef ARAFILTERTOOLS_H
#define ARAFILTERTOOLS_H

//ROOT includes
#include "Rtypes.h"

//AraRoot includes
#include "araAtriStructures.h"

class AraEventCalibrator;
class UsefulAtriStationEvent;
class RawAtriStationEvent;

#define FILTER_VERSION 2
/*
  THIS_FILTER should be set as follows:
  THIS_FILTER 0 - Random Filter
  THIS_FILTER 1 - Min-Bias Filter
  THIS_FILTER 2 - Time Sequence Filter
  THIS_FILTER 3 - NChnl Filter
  THIS_FILTER 4 - Track Engine Filter
  THIS_FILTER 5 - calPulser Filter
  THIS_FILTER 6-9 - Filters not yet defined
 */
#define ARA_FILTER_RANDOM 0
#define ARA_FILTER_MIN_BIAS 1
#define ARA_FILTER_TIME_SEQUENCE 2
#define ARA_FILTER_NCHNL_FILTER 3
#define ARA_FILTER_TRACK_ENGINE 4
#define ARA_FILTER_CALPULSER_PICKER 5
#define ARA_FILTER_UNDEFINED_6 6
#define ARA_FILTER_UNDEFINED_7 7
#define ARA_FILTER_UNDEFINED_8 8
#define ARA_FILTER_UNDEFINED_9 9


class TTree;
class TH1D;
class TRandom3;


//Here are the function prototypes
void runFilter(char *fileListName, char *outDir, Int_t runNumber, char *monitoringFileName, Int_t numEvents, char *pedFileName, Int_t thisFilter, TRandom3 *rand);
void processEvents(char *fileListName, TTree *qualityTree, char *pedFileName, TRandom3 *rand, int * EV_NUMBER_ARRAY);
TH1D* getHistoFromTree(TTree* theTree);
Double_t getCutValue(TH1D* qualityHisto, Int_t numberEvents);
Double_t getCutValue(TTree* qualityTree, Int_t numberEvents);
Double_t getQualityParameter(RawAtriStationEvent *evPtr, TRandom3 *rand, int * EV_NUMBER_ARRAY);
void updateFilterFlagBasedOnCut(char *fileListName, char *outputName, Int_t runNumber, Double_t cutValue, TTree *qualityTree, Int_t thisFilter);
void turnOnFilterFlag(AraStationEventHeader_t &theHeader);
void setFilterVersion(AraStationEventHeader_t &theHeader);
void setFilterFlagForThisFilter(AraStationEventHeader_t &theHeader, Int_t thisFilter);
Int_t getStationId(char *fileListName);
void setPedestalFile(char *fileListName, char *pedFileName, AraEventCalibrator *theCalibrator);
UShort_t getFilterFlagFromHeader(AraStationEventHeader_t &theHeader);
Int_t pickEvent(AraStationEventHeader_t &theEventHeader);
Int_t hasBitSetInFilterFlag(AraStationEventHeader_t &theEventHeader, Int_t bit);
Int_t hasOneOrMoreBitsSetInFilterFlag(AraStationEventHeader_t &theEventHeader);
TTree *sortTreeEntries(TTree *testTree);
double getWaveRms(UsefulAtriStationEvent *theEvent, double *chRms);
#endif //ARAFILTERTOOLS_H
