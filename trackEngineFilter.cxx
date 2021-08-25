#include <iostream>
#include <libgen.h>

#include "AraFilterTools.h"

//ROOT includes
#include "TTree.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TMath.h"
#include "TH2D.h"
#include "TFile.h"

//User's includes
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include "AraGeomTool.h"
#include "AraAntennaInfo.h"

//AraEvent includes
#include "RawAtriStationEvent.h"
#include "UsefulAraStationEvent.h"
#include "UsefulAtriStationEvent.h"
#include "AraStationInfo.h"

//FFTtools
#include "FFTtools.h"

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

#define THIS_FILTER ARA_FILTER_TRACK_ENGINE

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

////////////////////////////////////////////////////////////////////////////////////
//////  This is where the users code goes
////////////////////////////////////////////////////////////////////////////////////
/*RawIcrrStationEvent *rawIcrrEvPtr;
RawAtriStationEvent *rawAtriEvPtr;
RawAraStationEvent *rawEvPtr;
UsefulIcrrStationEvent *realIcrrEvPtr;
UsefulAtriStationEvent *realAtriEvPtr;
*/
int getMaxBin(UsefulAtriStationEvent *theEvent, int chnl);
double getMeanInNoMax(UsefulAtriStationEvent *theEvent, int chnl);
double getSigmaInNoMax(UsefulAtriStationEvent *theEvent, int chnl);
double getHitTime(UsefulAtriStationEvent *theEvent, int chnl, double threshold);
vector< vector<double> > buildComponentVectors(UsefulAtriStationEvent *theEvent);
vector<double> buildEventVector(UsefulAtriStationEvent *theEvent, const vector< vector<double> >& componentVectors, double threshold, double tolerance);

//////////////////////////////////////////////////////////////////////////////////
////
////  You must have the following functions:
////
////    Double_t getQualityParameter(UsefulAtriStationEvent *evPtr);
////    
//////////////////////////////////////////////////////////////////////////////////


Double_t getQualityParameter(UsefulAtriStationEvent *evPtr ){

   double threshold = 3.0;  //FILTER VOLTAGE THRESHOLD
   double tolerance = 1.001;//TOLERATED EXCESSIVE ACTUAL dT ( MEASURED BY ACTUAL dT / THEORETICAL dT )
   
   bool calCheck = evPtr->isCalpulserEvent();;

   static vector< vector<double> > componentVectors = buildComponentVectors(evPtr);
   vector<double> eventVector;

   //componentVectors = buildComponentVectors(evPtr);

//   if ( !calCheck ){   
      
      eventVector = buildEventVector(evPtr,componentVectors, threshold, tolerance);
      double eventVectorLength = sqrt(eventVector[0]*eventVector[0]+eventVector[1]*eventVector[1]+eventVector[2]*eventVector[2]);

//   }

return eventVectorLength;
}


//*******************************************************************************
//FUNCTIONS
//*******************************************************************************

int getMaxBin(UsefulAtriStationEvent *theEvent, int chnl){

   Double_t *volts = 0;
   //Double_t *times = 0;
   int bin = 0;

   TGraph *gr = theEvent->getGraphFromRFChan(chnl);
   volts = gr->GetY();
   //times = gr->GetX();
   bin   = gr->GetN();
   double max = 0;
   int MaxBin = 0;

   for ( int  i=0; i<bin; i++){

   if ( fabs(volts[i]) > max ){
      max = fabs(volts[i]);
      MaxBin = i;      
   }
   } 
   //cout<<"MaxBin="<<MaxBin<<"Max Times="<<times[MaxBin]<<endl;
   delete gr;
   return MaxBin;
}

double getMeanInNoMax(UsefulAtriStationEvent *theEvent, int chnl){

   Double_t *volts = 0;
   int bin=0;
   int MaxBin = getMaxBin(theEvent, chnl);
   int binCounter=0;   

   TGraph *gr = theEvent->getGraphFromRFChan(chnl);
   volts = gr->GetY();
   bin   = gr->GetN();
   //printf("bin=%d\tbin from times array=%d\nbin from volts array=%d\n",bin,sizeof(times)/sizeof(times[0]),sizeof(volts)/sizeof(volts[0]));
   double mean = 0;
 
   if( MaxBin <= bin/4 ){

      for (int i=MaxBin+bin/4; i<bin; i++){
      mean+=volts[i];
      binCounter++;
      }
   }

   else if( MaxBin >= 3*bin/4 ){

      for (int i=0; i<MaxBin-bin/4; i++){
      mean+=volts[i];
      binCounter++;
      }      
   }

   else{
   
      for (int i=0; i<MaxBin-bin/4; i++){
      mean+=volts[i];
      binCounter++;
      }

      for (int i=MaxBin+bin/4; i<bin; i++){
      mean+=volts[i];
      binCounter++;
      }
   }


   mean=mean/binCounter;

   delete gr;

   return mean;
}

double getSigmaInNoMax(UsefulAtriStationEvent *theEvent, int chnl){

   Double_t *volts=0;
   int bin=0;
   int MaxBin = getMaxBin(theEvent, chnl);
   int binCounter=0;   
   double mean = getMeanInNoMax(theEvent, chnl); 

   TGraph *gr = theEvent->getGraphFromRFChan(chnl);
   volts = gr->GetY();
   bin   = gr->GetN();
   
   double sigma=0;

   if( MaxBin <= bin/4 ){

      for (int i=MaxBin+bin/4; i<bin; i++){
      sigma += volts[i] * volts[i];
      binCounter++;
      }
   }

   else if( MaxBin >= 3*bin/4 ){

      for (int i=0; i<MaxBin-bin/4; i++){
      sigma += volts[i] * volts[i];
      binCounter++;
      }      
   }

   else{
   
      for (int i=0; i<MaxBin-bin/4; i++){
      sigma += volts[i] * volts[i];
      binCounter++;
      }

      for (int i=MaxBin+bin/4; i<bin; i++){
      sigma += volts[i] * volts[i];
      binCounter++;
      }
   }
  
   sigma = TMath::Sqrt( ( sigma - ((double)binCounter * mean * mean )) / (double)(binCounter - 1) );
   //cout<<"mean="<<mean<<"\tsigma="<<sigma<<endl;
   delete gr;

   return sigma;
}

double getHitTime(UsefulAtriStationEvent *theEvent, int chnl, double threshold){

   Double_t *volts = 0;
   Double_t *times = 0;
   int bin = 0;
   double mean  = getMeanInNoMax(theEvent, chnl);
   double sigma = getSigmaInNoMax(theEvent, chnl);
   //double threshold = 5.5;

   TGraph *gr = theEvent->getGraphFromRFChan(chnl);
   volts = gr->GetY();
   times = gr->GetX();
   bin   = gr->GetN();   

   //double max = threshold * sigma;
   double ht  = -1; //IF RETURNS -1, CHANNEL WILL NOT BE USED IN ALGORITHM
   double max = 0;

   for (int binCounter=0; binCounter<bin; binCounter++){

      if ( volts[binCounter] > (mean + threshold * sigma) && fabs(volts[binCounter]) > max ){
         max = fabs(volts[binCounter]);
         ht  = times[binCounter];
      } 
      else if ( volts[binCounter] < (mean - threshold * sigma) && fabs(volts[binCounter]) > max ){
         max = fabs(volts[binCounter]);
         ht  = times[binCounter];
      }
   }
   
   delete gr;

return ht; //IN UNIT OF? NOW ASSUME NANOSEC
}

vector< vector<double> > buildComponentVectors(UsefulAtriStationEvent *theEvent){

   int stationId = theEvent->stationId;
   
   AraGeomTool *tempGeom = AraGeomTool::Instance();
   

   int numAnts = tempGeom->getStationInfo(stationId)->getNumAntennasByPol(AraAntPol::kVertical) + tempGeom->getStationInfo(stationId)->getNumAntennasByPol(AraAntPol::kHorizontal);
   //cout<<"numAnts="<<numAnts<<endl;
   int numVectors = numAnts * numAnts; 
   double index = 1.76;     //FIXME
   double lightSpeed = TMath::C()*1e-9; //IN UNITS m/ns
   double dt, dx, dy, dz, dr;   
   //cout<<"lightSpeed="<<lightSpeed<<endl;
   vector< vector<double> > componentVectors;
   vector<double> singleVector[numVectors];
   
   for (int vecCount =0; vecCount<numVectors; vecCount++){

      componentVectors.push_back(singleVector[vecCount]);
      dx=tempGeom->getStationInfo(stationId)->getAntennaInfo(vecCount%numAnts)->antLocation[0]-tempGeom->getStationInfo(stationId)->getAntennaInfo(vecCount/numAnts)->antLocation[0];
      dy=tempGeom->getStationInfo(stationId)->getAntennaInfo(vecCount%numAnts)->antLocation[1]-tempGeom->getStationInfo(stationId)->getAntennaInfo(vecCount/numAnts)->antLocation[1];
      dz=tempGeom->getStationInfo(stationId)->getAntennaInfo(vecCount%numAnts)->antLocation[2]-tempGeom->getStationInfo(stationId)->getAntennaInfo(vecCount/numAnts)->antLocation[2];
      dr = sqrt( dx*dx + dy*dy + dz*dz );
      if ( dr != 0 ){
         dx /= dr;
         dy /= dr;
         dz /= dr;    
         dt  = dr * index / lightSpeed;
      } else {
         dt  = 0;
      }   
      //printf("dx=%f\tdy=%f\tdz=%f\tdr=%f\tdx^2+dy^2+dz^2=%f\tdt=%f\n",dx,dy,dz,dr,dx*dx+dy*dy+dz*dz,dt);
      componentVectors.at(vecCount).push_back(dt);      
      componentVectors.at(vecCount).push_back(dx);      
      componentVectors.at(vecCount).push_back(dy);      
      componentVectors.at(vecCount).push_back(dz);      

   }
      
return componentVectors;
}

vector<double> buildEventVector(UsefulAtriStationEvent *theEvent, const vector< vector<double> >& componentVectors, double threshold, double tolerance){

   AraGeomTool *tempGeom = AraGeomTool::Instance();
   int stationId = theEvent->stationId;
   int numAnts=tempGeom->getStationInfo(stationId)->getNumAntennasByPol(AraAntPol::kVertical)+tempGeom->getStationInfo(stationId)->getNumAntennasByPol(AraAntPol::kHorizontal);
   double eventVecX, eventVecY, eventVecZ;
   eventVecX=eventVecY=eventVecZ=0;

//BUILDING VECTOR CONTAINING ALL 16 HITTIMES
   vector<double> htVector;
   double hitTime = -1;

   for (int ch=0; ch<numAnts; ch++){
      
      hitTime = getHitTime(theEvent,ch, threshold);
      htVector.push_back(hitTime);
      
      //cout<<"ht="<<htVector.at(ch)<<endl;
   }   
//HITTIME VECTOR BUILT

   double deltaHt = 0;
   double cosine  = 0;
   int pairCount  = 0;

   for (int ch_1=0; ch_1<numAnts; ch_1++){
      for( int ch_2=0; ch_2 < ch_1; ch_2++){

         if (htVector[ch_1] >0 && htVector[ch_2]>0 ){

            pairCount++;

            if ( htVector[ch_1] > htVector[ch_2] ){   //CH_1 IS LATER THAN CH_2

            deltaHt = htVector[ch_1] - htVector[ch_2];
            cosine = deltaHt / componentVectors[numAnts * ch_2 + ch_1 ][0];
            
            if (cosine > 1){ 
               if ( cosine > tolerance ){
               cosine = 0;
               pairCount--;
               } else {
               cosine = 1;
               }
            }
     
            eventVecX += componentVectors[numAnts*ch_2+ch_1][1] * cosine;
            eventVecY += componentVectors[numAnts*ch_2+ch_1][2] * cosine;
            eventVecZ += componentVectors[numAnts*ch_2+ch_1][3] * cosine;

            } else { //CH_2 IS LATER THAN CH_1
            
            deltaHt = htVector[ch_2] - htVector[ch_1];
            cosine = deltaHt / componentVectors[numAnts * ch_1 + ch_2 ][0];

            if (cosine > 1){ 
               if ( cosine > tolerance ){
               cosine = 0;
               pairCount--;
               } else {
               cosine = 1;
               }
            }

            eventVecX += componentVectors[numAnts*ch_1+ch_2][1] * cosine;
            eventVecY += componentVectors[numAnts*ch_1+ch_2][2] * cosine;
            eventVecZ += componentVectors[numAnts*ch_1+ch_2][3] * cosine;

            }
         }    
      }
   }

   double normalization;

   if ( pairCount != 0 ){
   
   normalization = 1;
   //normalization = sqrt(pairCount);
   eventVecX /= normalization;
   eventVecY /= normalization;
   eventVecZ /= normalization;
   
   }

   vector<double> eventVector; 
   eventVector.push_back(eventVecX);
   eventVector.push_back(eventVecY);
   eventVector.push_back(eventVecZ);

return eventVector;
}

