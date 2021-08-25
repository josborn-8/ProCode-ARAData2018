
#include "AraFilterTools.h"

//General includes
#include <cstdio>
#include <fstream>
#include <iostream>
#include <zlib.h>
#include <libgen.h>     
#include <cstdlib>
#include <time.h>

//AraRoot includes
#include "araAtriStructures.h"
#include "RawAtriStationEvent.h"  
#include "UsefulAtriStationEvent.h"  

//ROOT includes
#include "TMath.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "TRandom3.h"

//File writer includes -- this is used for writing the data out
extern "C" {
   #include "fileWriterUtil.h"
   #include "syslog.h"
}

FILE *fLogFile;

using namespace std;




void runFilter(char *fileListName,  char *outDir, Int_t runNumber,  char *monitoringFileName, Int_t numEvents, char *pedFileName, Int_t thisFilter, TRandom3 *rand) {
  char outputName[FILENAME_MAX];  
  sprintf(outputName, "%s/run_%06d/event", outDir, runNumber);
  

  cout << fileListName << "\t" << outputName << endl;

  Int_t stationId = getStationId(fileListName);

  char logFileName[FILENAME_MAX];
  char stationName[FILENAME_MAX];
  char filterName[FILENAME_MAX];
  if(stationId==2) sprintf(stationName, "ARA02");
  else if(stationId==3) sprintf(stationName, "ARA03");

  
  /*
    #define ARA_FILTER_RANDOM 0
    #define ARA_FILTER_MIN_BIAS 1
    #define ARA_FILTER_TIME_SEQUENCE 2
    #define ARA_FILTER_NCHNL_FILTER 3
    #define ARA_FILTER_TRACK_ENGINE 4
    #define ARA_FILTER_UNDEFINED_5 5
    #define ARA_FILTER_UNDEFINED_6 6
    #define ARA_FILTER_UNDEFINED_7 7
    #define ARA_FILTER_UNDEFINED_8 8
    #define ARA_FILTER_UNDEFINED_9 9
*/  
  switch(thisFilter)
    {
    case ARA_FILTER_RANDOM:
      sprintf(filterName, "ARA_FILTER_RANDOM");
      break;
    case ARA_FILTER_MIN_BIAS:
      sprintf(filterName, "ARA_FILTER_MIN_BIAS");
      break;
    case ARA_FILTER_TIME_SEQUENCE:
      sprintf(filterName, "ARA_FILTER_TIME_SEQUENCE");
      break;
    case ARA_FILTER_NCHNL_FILTER:
      sprintf(filterName, "ARA_FILTER_NCHNL_FILTER");
      break;
    case ARA_FILTER_TRACK_ENGINE:
      sprintf(filterName, "ARA_FILTER_TRACK_ENGINE");
      break;
    case ARA_FILTER_CALPULSER_PICKER:
      sprintf(filterName, "ARA_FILTER_CALPULSER_PICKER");
      break;
    default:
      sprintf(filterName, "ARA_FILTER_DUNNO");
      break;
    }
  
  int EV_NUMBER_ARRAY[1000000] = {0};
  TTree *qualityTree = new TTree("qualityTree", "Tree with event number and filter quality parameter");
  processEvents(fileListName, qualityTree, pedFileName, rand, &EV_NUMBER_ARRAY[0]);
  TH1D* qualityHisto = getHistoFromTree(qualityTree);

 //  Double_t cutValue = getCutValue(qualityHisto, numEvents);
  Double_t cutValue = 1.0;///getCutValue(qualityTree, numEvents);

  //update the flag and write out the events  
  updateFilterFlagBasedOnCut(fileListName, outputName, runNumber, cutValue, qualityTree, thisFilter);

  //Write out monitoring file
  TFile *fpOut = new TFile(monitoringFileName, "RECREATE");
  fpOut->cd();
  qualityTree->Write();
  qualityHisto->Write();
  fpOut->Close();

}

void processEvents(char *fileListName, TTree *qualityTree, char *pedFileName, TRandom3 *rand, int * EV_NUMBER_ARRAY){

  char *inBuffer;
  inBuffer = new char[200000];

  RawAtriStationEvent *theEvent=0;
//  UsefulAtriStationEvent *theUsefulEvent=0;
  AraStationEventHeader_t theEventHeader;

  //Stuff for the tree
  Double_t qualityParameter=0;
  Int_t eventNumber=0;
  qualityTree->Branch("qualityParameter", &qualityParameter, "qualityParameter/D");
  qualityTree->Branch("eventNumber", &eventNumber, "eventNumber/I");

//  Setup the pedestalFile
//  AraEventCalibrator *theCalibrator=0;
//  setPedestalFile(fileListName, pedFileName, theCalibrator);
  cout << "Starting the loop!" << endl;

  int numBytes=0;
  char fileName[180];
  int error=0;
  int counter=0;
  time_t timer1;
  time_t timer2;
  time(&timer1);
  int numEventsRead=0;
  ifstream SillyFile(fileListName);  
  while(SillyFile >> fileName) {
    gzFile infile = gzopen (fileName, "rb");    

    while (1){
      numBytes = gzread(infile,&theEventHeader,sizeof(AraStationEventHeader_t));
      if(numBytes == 0 ){
	//	fprintf(stderr, "Finished reading file %s\n", fileName);
	break;
      }
      if(theEventHeader.gHdr.numBytes>0) {
	Int_t numDataBytes=theEventHeader.gHdr.numBytes-sizeof(AraStationEventHeader_t);
	numBytes=gzread(infile,inBuffer,numDataBytes);
	numEventsRead++;
	if(numBytes==0) {
	  fprintf(stderr, "Got header but no data from file %s\n", fileName);
	  break;
	}
  	if(numBytes!=numDataBytes) {
	  if(numBytes)
	    cerr << "Read problem: " <<numBytes << " of " <<  numDataBytes << endl;
	  error=1;
	  break;
	}

	//Build the event
	theEvent = new RawAtriStationEvent(&theEventHeader,inBuffer);
//	theUsefulEvent = new UsefulAtriStationEvent(theEvent, AraCalType::kLatestCalib);
	qualityParameter = getQualityParameter(theEvent, rand, &EV_NUMBER_ARRAY[0]);
    	counter++;
	eventNumber = theEvent->eventNumber;
	qualityTree->Fill();
	
	delete(theEvent);
//	delete(theUsefulEvent);
      }
      else {
	std::cerr << "How can gHdr.numBytes = " << theEventHeader.gHdr.numBytes << "\n";
	error=1;
	break;
      }
    }
    gzclose(infile);
  }
   time(&timer2);
  double seconds = difftime(timer2,timer1);
   cout << "Time diff: " << seconds << endl;
  qualityTree->ResetBranchAddresses();
//  if(theCalibrator) delete theCalibrator;
  delete [] inBuffer;
}
  
  
TH1D* getHistoFromTree(TTree* theTree){
  TCanvas *tempCan = new TCanvas();
  theTree->Draw("qualityParameter>>histo(200)");
    TH1D *histo = (TH1D*)gDirectory->Get("histo");
//  TH1D* tempHist = (TH1D*) tempCan->GetPrimitive("htemp");
//  TH1D* histo = (TH1D*) tempHist->Clone("qualityHist");

  delete tempCan;
  return histo;  
}

Double_t getCutValue(TH1D* qualityHisto, int numberOfEvents)
{

  int nBins = qualityHisto->GetNbinsX();
  Double_t total_count = qualityHisto->Integral(0,nBins);
  Double_t count = 0;
  Double_t cut_value = 0;
  for(int i=1; i<nBins;i++)
    {
      count = total_count - qualityHisto->Integral(0,i);
      if(count<=numberOfEvents)
	{
	  cut_value = qualityHisto->GetBinLowEdge(i+1);
	  return cut_value;
	}
    }
  return -1;
}

Double_t getCutValue(TTree *qualityTree, int numberOfEvents)
{

  TTree *sortedTree = sortTreeEntries(qualityTree);
  Int_t nentries = (Int_t)sortedTree->GetEntries();
  sortedTree->Draw("qualityParameter", "", "goff");
  Double_t *qualityParameter = sortedTree->GetV1();
  
  if(nentries<numberOfEvents) numberOfEvents = nentries;
  
  Double_t cutValue = qualityParameter[numberOfEvents];

  delete sortedTree;
  return cutValue;
}

void updateFilterFlagBasedOnCut(char *fileListName, char *outputName, Int_t runNumber, Double_t cutValue, TTree *qualityTree, Int_t thisFilter){
  char *inBuffer;
  char *outBuffer;
  
  inBuffer = new char[200000];
  outBuffer = new char[200000];
  AraStationEventHeader_t theEventHeader;

  ARAWriterStruct_t eventWriter;

  int numBytes=0;
  char fileName[180];
  int error=0;
  int counter=0;
  Int_t numEventsRead=0;
  Int_t treeEntry=0;

  //Set the branches in the Tree
  Double_t qualityParameter=0;
  Int_t eventNumber=0;
  qualityTree->SetBranchAddress("qualityParameter", &qualityParameter);
  qualityTree->SetBranchAddress("eventNumber", &eventNumber);

  ifstream SillyFile(fileListName);  
  while(SillyFile >> fileName) {
    counter++;
    //    fprintf(stderr, "fileName %s\n", fileName);
    gzFile infile = gzopen (fileName, "rb");    
    
    while (1) {
      numBytes = gzread(infile,&theEventHeader,sizeof(AraStationEventHeader_t));
      if(numBytes == 0 ){
	//	fprintf(stderr, "Finished reading file %s\n", fileName);
	break;
      }
      else if(numBytes != sizeof(AraStationEventHeader_t)){
	  fprintf(stderr, "Arggh! Wrong size read\n");
	  break;
	}
      if(theEventHeader.gHdr.numBytes>0) {
	Int_t numDataBytes=theEventHeader.gHdr.numBytes-sizeof(AraStationEventHeader_t);
	numBytes=gzread(infile,inBuffer,numDataBytes);
	numEventsRead++;
	if(numEventsRead%100==0){
	  //fprintf(fLogFile, "%s -- eventNumber %i\n", __FUNCTION__, numEventsRead);
	  //fflush(fLogFile);
	}
	if(numBytes != numDataBytes)
	  fprintf(stderr, "event %i numDataBytes %i numDataBytes - numBytes %i\n", 
		  theEventHeader.eventNumber,
		  numDataBytes,
		  numDataBytes - numBytes);
	if(numBytes==0) {
	  fprintf(stderr, "Got header but no data from file %s\n", fileName);
	  break;
	}
	if(numBytes!=numDataBytes){
	  if(numBytes)
	    cerr << "Read problem: " <<numBytes << " of " <<  numDataBytes << endl;
	  error=1;
	  break;
	}
	//Build the event
	//And write it out
	static int doneInit=0;
	static int new_file_flag=0; //FIXME
	if(!doneInit) {
	  initWriter(&eventWriter,
		     runNumber,
		     5,
		     100,
		     100,
		     EVENT_FILE_HEAD,
		     outputName,
		     NULL);    
	  doneInit=1;
	}  
	int numToCopy = theEventHeader.gHdr.numBytes;
	int upToByte = sizeof(AraStationEventHeader_t);
	

	//Get the quality parameter for this event
	qualityTree->GetEntry(treeEntry);
	treeEntry++;

	//Turn on the filterFlag
	turnOnFilterFlag(theEventHeader);
	setFilterVersion(theEventHeader);

	if(qualityParameter > cutValue){
	  // fprintf(stderr, "eventNumber (tree) %i (event) %i qualPar (tree) %f cutValue %f\n",
	  // 	  eventNumber, theEventHeader.eventNumber, 
	  // 	  qualityParameter, cutValue);
	  setFilterFlagForThisFilter(theEventHeader, thisFilter);
	}

	memcpy( &outBuffer[0],&theEventHeader , sizeof(AraStationEventHeader_t)); 
	memcpy( &outBuffer[upToByte], inBuffer, numToCopy);
	
	int retVal = writeBuffer( &eventWriter, outBuffer, numToCopy, &(new_file_flag));

      }
      else {
	std::cerr << "How can gHdr.numBytes = " << theEventHeader.gHdr.numBytes << "\n";
	error=1;
	break;
      }
    }
    gzclose(infile);
  }

  closeWriter(&eventWriter);
  qualityTree->ResetBranchAddresses();
  SillyFile.close();
  delete [] inBuffer;
  delete [] outBuffer;
}


void turnOnFilterFlag(AraStationEventHeader_t &theHeader)
{
  UShort_t filterFlag = getFilterFlagFromHeader(theHeader);
  
  if((filterFlag & 0x0001)==0)//This means the filter flag is not set
    filterFlag = 0x0001;
  
  theHeader.gHdr.reserved = filterFlag;
}

void setFilterVersion(AraStationEventHeader_t &theHeader)
{
  //5 bits for the filter version
  // 0x3e   == 0000 0000 0011 1110
  // 0xffc1 == 1111 1111 1100 0001
  
  UShort_t filterFlag = getFilterFlagFromHeader(theHeader);
  Int_t version = FILTER_VERSION;
  version = version << 1;
  version = (version) & 0x003e;
  version = (version) | 0x0001;

  //Set the 5 bits for version to zero
  filterFlag = (filterFlag) & 0xffc1;
  
  //Now add in the 5 bit version
  filterFlag = (filterFlag) | version;

  theHeader.gHdr.reserved = filterFlag;
}


void setFilterFlagForThisFilter(AraStationEventHeader_t &theHeader, Int_t thisFilter)
{
  UShort_t filterFlag = getFilterFlagFromHeader(theHeader);

  Int_t thisFilterBit = thisFilter;
  Int_t filterBitMask = 0x01 << thisFilterBit;
  //10 bits for filter mask
  // 0xffc0  == 1111 1111 1100 000

  filterBitMask = filterBitMask << 6;
  filterBitMask = (filterBitMask) & 0xffc0; 

  filterFlag = (filterFlag) | filterBitMask;

  theHeader.gHdr.reserved = filterFlag;
}

void setPedestalFile(char *fileListName, char *pedFileName, AraEventCalibrator *theCalibrator){
  //Get the station Id from the first event in the first file
  char fileName[FILENAME_MAX];
  AraStationEventHeader_t theEventHeader;
  ifstream SillyFile(fileListName);  
  SillyFile >> fileName;

  //  fprintf(stderr, "%s file %s\n", __FUNCTION__, pedFileName);

  gzFile infile = gzopen (fileName, "rb");    
  Int_t numBytes=gzread(infile,&theEventHeader,sizeof(AraStationEventHeader_t));
  Int_t stationId = theEventHeader.gHdr.stationId;
  gzclose(infile);

  //Set the pedestal file  
  theCalibrator = AraEventCalibrator::Instance();
  theCalibrator->setAtriPedFile(pedFileName, stationId);

  SillyFile.close();
  
}


Int_t getStationId(char *fileListName){
  //Get the station Id from the first event in the first file
  char fileName[FILENAME_MAX];
  AraStationEventHeader_t theEventHeader;
  ifstream SillyFile(fileListName);  
  SillyFile >> fileName;

  //  fprintf(stderr, "%s file %s\n", __FUNCTION__, pedFileName);

  gzFile infile = gzopen (fileName, "rb");    
  Int_t numBytes=gzread(infile,&theEventHeader,sizeof(AraStationEventHeader_t));
  Int_t stationId = theEventHeader.gHdr.stationId;

  SillyFile.close();

  return stationId;
}



UShort_t getFilterFlagFromHeader(AraStationEventHeader_t &theHeader)
{
  return theHeader.gHdr.reserved;
}


Int_t pickEvent(AraStationEventHeader_t &theEventHeader){
  return hasOneOrMoreBitsSetInFilterFlag(theEventHeader);
}
Int_t hasBitSetInFilterFlag(AraStationEventHeader_t &theEventHeader, Int_t bit){
  if(bit > 9){
    fprintf(stderr, "%s -- error - bit greater than 9\n", __FUNCTION__);
    return -1;
  }
  Int_t bitMask = (0x01) << bit;
  UShort_t filterFlag = getFilterFlagFromHeader(theEventHeader);
  Int_t theFlag = (filterFlag >> 6) &0x3ff;
  
  if( (bitMask & theFlag) == bitMask ) return 1;
  else return 0;
}
Int_t hasOneOrMoreBitsSetInFilterFlag(AraStationEventHeader_t &theEventHeader){
  Int_t bit=0;
  for(bit=0;bit<10;bit++){
    if(hasBitSetInFilterFlag(theEventHeader, bit) == 1) return 1;
  }
  return 0;
}

TTree *sortTreeEntries(TTree *testTree)
{
	Int_t nentries = (Int_t)testTree->GetEntries();
	testTree->Draw("qualityParameter","","goff");
	Int_t *index = new Int_t[nentries];
	TMath::Sort(nentries,testTree->GetV1(),index);
	TTree *tsorted = (TTree*)testTree->CloneTree(0);
		for (Int_t i=0;i<nentries;i++) {
		testTree->GetEntry(index[i]);
		tsorted->Fill();
	}
	delete [] index;
	return tsorted;
}


double getWaveRms(UsefulAtriStationEvent *theEvent, double *chRms)
{
	Double_t *times = 0;
	Double_t *volts = 0;
	int bin = 0;
	double ch_energy = 0.0;
	double av_rms = 0.0;
	for(int rfchan=0;rfchan<16;rfchan++)
	{//loop channels
		ch_energy = 0.0;
		TGraph *gr = theEvent->getGraphFromRFChan(rfchan);
		times = gr->GetX();
		volts = gr->GetY();
		bin = gr->GetN()-1;
                for (int l=0; l<bin; l++) 
		{//loop datapoints
			ch_energy += volts[l]*volts[l];
                }//end loop datapoints
		chRms[rfchan] = ch_energy/bin;
		av_rms +=ch_energy/bin;
	}//end loop channels
	av_rms = av_rms/16.0;
  return av_rms;
}
