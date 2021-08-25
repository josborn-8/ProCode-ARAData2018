#include "Rtypes.h"

//General includes
#include <cstdio>
#include <fstream>
#include <iostream>
#include <zlib.h>
#include <libgen.h>     
#include <cstdlib>

//AraRoot includes
#include "araAtriStructures.h"
#include "AraFilterTools.h"
#include "TRandom3.h"


//File writer includes -- this is used for writing the data out
extern "C" {
   #include "fileWriterUtil.h"
   #include "syslog.h"
}

void runFilter(char *fileListName,  char *outDir, Int_t runNumber);

using namespace std;

int main(int argc, char **argv) {
  if(argc<4) {
    std::cout << "Usage: " << basename(argv[0]) << " <fileListName> <outDir> <runNumber>" << std::endl;
    return -1;
  }
  Int_t runNumber=atoi(argv[3]);
  runFilter(argv[1],argv[2], runNumber);
  return 0;
}

void runFilter(char *fileListName,  char *outDir, Int_t runNumber){
  char outputName[FILENAME_MAX];
  sprintf(outputName, "%s/run_%06d/event", outDir, runNumber);


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
  Int_t treeEntry=0;
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

	if(pickEvent(theEventHeader) == 1){
	  memcpy( &outBuffer[0],&theEventHeader , sizeof(AraStationEventHeader_t)); 
	  memcpy( &outBuffer[upToByte], inBuffer, numToCopy);
	  int retVal = writeBuffer( &eventWriter, outBuffer, numToCopy, &(new_file_flag));
	}
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
  SillyFile.close();
  delete [] inBuffer;
  delete [] outBuffer;
}


//Argh! Just to make this compile!
Double_t getQualityParameter(RawAtriStationEvent *evPtr, TRandom3 *rand, int * EV_NUMBER_ARRAY){
  //Do something clever
  return 0;
}

