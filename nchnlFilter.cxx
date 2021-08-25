#include <iostream>
#include <libgen.h>     

#include "AraFilterTools.h"
#include "UsefulAtriStationEvent.h"


//ROOT Includes
#include "TTree.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TMath.h"
#include "TH2D.h"

//AraEvent includes
#include "RawAtriStationEvent.h"
#include "UsefulAraStationEvent.h"
#include "UsefulAtriStationEvent.h"
#include "AraStationInfo.h"

//FFTtools
#include "FFTtools.h"

//User's includes
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include "AraGeomTool.h"
#include "AraAntennaInfo.h"

#include "TRandom3.h"
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

#define THIS_FILTER ARA_FILTER_NCHNL_FILTER

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
////  This is where the users code goes
//////////////////////////////////////////////////////////////////////////////////

int getMaxBin(Double_t *, int);
void setMeanAndSigmaInNoMax(Double_t *, int, double *);

////////////////////////////////////////////////////////////////////////////////
//
//  You must have the following functions:
//
//    Double_t getQualityParameter(UsefulAtriStationEvent *evPtr);
//    
////////////////////////////////////////////////////////////////////////////////

int getMaxBin(Double_t *volts, int bin){

   //Double_t *volts = 0;
   //int bin = 0;

   //TGraph *gr = theEvent->getGraphFromRFChan(chnl);
   //volts = gr->GetY();
   //bin   = gr->GetN();
   double max = 0;
   int MaxBin = 0;

   for ( int  i=0; i<bin; i++){

   if ( fabs(volts[i]) > max ){
      max = fabs(volts[i]);
      MaxBin = i;      
   }
   }

   //delete gr;
   return MaxBin;
}

void setMeanAndSigmaInNoMax(Double_t *volts, int bin, double *stats){

   //Double_t *volts=0;
   //int bin=0;
   int MaxBin = getMaxBin(volts, bin);
   int binCounter=0;   

   //TGraph *gr = theEvent->getGraphFromRFChan(chnl);
   //volts = gr->GetY();
   //bin   = gr->GetN();
  
   double mean =0; 
   double sigma=0;

   if( MaxBin <= bin/4 ){

      for (int i=MaxBin+bin/4; i<bin; i++){
      mean  += volts[i];
      sigma += volts[i] * volts[i];
      binCounter++;
      }   
   }   

   else if( MaxBin >= 3*bin/4 ){

      for (int i=0; i<MaxBin-bin/4; i++){
      mean  += volts[i];
      sigma += volts[i] * volts[i];
      binCounter++;
      }          
   }   

   else{
   
      for (int i=0; i<MaxBin-bin/4; i++){
      mean  += volts[i];
      sigma += volts[i] * volts[i];
      binCounter++;
      }   

      for (int i=MaxBin+bin/4; i<bin; i++){
      mean  += volts[i];
      sigma += volts[i] * volts[i];
      binCounter++;
      }   
   }
   mean  = mean / binCounter;
   sigma = TMath::Sqrt( ( sigma - ((double)binCounter * mean * mean )) / (double)(binCounter - 1) );
   //cout<<"mean="<<mean<<"\tsigma="<<sigma<<endl;
   //delete gr;
   
   stats[0] = mean;
   stats[1] = sigma;
}
 
Double_t getQualityParameter(UsefulAtriStationEvent *evPtr){
   TRandom3 random3(0);
   double threshold = 5.5;  //FILTER VOLTAGE THRESHOLD
   
   double sigma;
   double mean;
   double statsArray[2]={0};   //ELEMENT 0 SHOULD BE MEAN, ELEMENT 1 SHOULD BE SIGMA
   //TGraph *gr;
   Double_t *volts;
   int bin;
   double nChnl=0;

   bool cal_check = 0;
   cal_check = evPtr->isCalpulserEvent();
   
   if (!cal_check){
   
   TGraph *gr;
   for ( int ch=0; ch<16; ch++){

      gr = evPtr->getGraphFromRFChan(ch);
      volts = gr->GetY();
      bin   = gr->GetN();

      setMeanAndSigmaInNoMax(volts, bin, statsArray);

      mean  = statsArray[0];
      sigma = statsArray[1];
 

      for (int binCounter=0; binCounter<bin; binCounter++){

         if ( fabs(volts[binCounter] - mean) > threshold * sigma ){
    
            nChnl += 1;
            break;
         }   
      } 

      delete gr;
   }
   }
//   delete gr;

return nChnl + random3.Rndm();
}  

/*
Double_t getQualityParameter(UsefulAtriStationEvent *evPtr)
{

   double threshold = 5.5; //FILTER VOLTAGE THRESHOLD 
   double chnl;
   //bool cal_check=0;
   //cal_check = evPtr->isCalpulserEvent();

   //if(!cal_check)
   //{//loop all events

      chnl = getNchnl(evPtr,threshold) ;

   //}//end loop all events

return chnl;
}
*/
