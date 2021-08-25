#include <iostream>
#include <libgen.h>     

#include "AraFilterTools.h"
#include "UsefulAtriStationEvent.h"
//User's includes

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
#include "AraGeomTool.h"

//FFTtools
#include "FFTtools.h"

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

#define THIS_FILTER ARA_FILTER_TIME_SEQUENCE

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


double gen_trig_histo_single(std::vector<TGraph *> gr, TH2D *trig_pat, double *ch_std_dev, double *ch_average, double th_factor, int sum_time, int stationId);

//int gen_noise_std_dev(UsefulAtriStationEvent *theEvent, double *ch_noise_std_dev, double *ch_noise_average, int sum_time);
int gen_noise_std_dev(std::vector<TGraph *> gr, double *ch_noise_std_dev, double *ch_noise_average, int sum_time);

double pattern_check(TH2D *trig_pat, double *pat_check_result, int cut_value, std::vector< std::vector< double > > antloc);

int inter_string_check(TH2D *trig_pat, int *same_speed_t, int *same_speed_d1, int *same_speed_d2, int *same_speed_v, int *same_speed_h, int *c_s, int *c_d, std::vector< std::vector< double > > antloc);




////////////////////////////////////////////////////////////////////////////////
//
//  You must have the following functions:
//
//    Double_t getQualityParameter(UsefulAtriStationEvent *evPtr);
//    
////////////////////////////////////////////////////////////////////////////////


int inter_string_check(TH2D *trig_pat, int *same_speed_t, int *same_speed_d1, int *same_speed_d2, int *same_speed_v, int *same_speed_h, int *c_s, int *c_d, std::vector< std::vector< double > > antloc)
{
	double hit[8000][5];
	double string_pos[4];
	string_pos[0] = 1;
	string_pos[1] = 2;
	string_pos[2] = 3;
	string_pos[3] = 4;
	int max_bin = trig_pat->GetNbinsX();
	int count = 0;

	for(int ff=0;ff<800;ff++)
	{
		same_speed_t[ff] = 0.0;
		same_speed_d1[ff] = 0.0;
		same_speed_d2[ff] = 0.0;
		same_speed_v[ff] = 0.0;
		same_speed_h[ff] = 0.0;
		c_s[ff]=0.0;
		c_d[ff]=0.0;
	}

//	cout << "Bin number: " << max_bin << endl;

	for(int i=1;i<max_bin+1;i++)
	{
		for(int j=1;j<17;j++)
		{
			if(trig_pat->GetBinContent(i,j)>0)
			{
				hit[count][0] = i*500.0/max_bin;
				if(j==1) hit[count][1] = antloc[j-1][2] + 1.37;
				else hit[count][1] = antloc[j-1][2];
				hit[count][2] = antloc[j-1][0];
				hit[count][3] = antloc[j-1][1];
				hit[count][4] = string_pos[(j-1)%4];
				count++;
			 }
		}
	}

//actually 15 and 20 were good factors but things are changed to ten to make it easier to understand

	double distance = 0;

	for(int gg = 0; gg<count;gg++)
	{
		for(int kk=gg+1;kk<count;kk++)
		{
			if(TMath::Abs(hit[kk][1] - hit[gg][1])<1.6||hit[kk][1]==hit[gg][1])
			{ 
				if(TMath::Sqrt( (hit[kk][2]-hit[gg][2])*(hit[kk][2]-hit[gg][2]) + (hit[kk][3]-hit[gg][3])*(hit[kk][3]-hit[gg][3]) )!=0)
				{
					if(TMath::Abs(hit[kk][4]-hit[gg][4])==1 && hit[kk][4]+hit[gg][4]!=5)
					{
						 distance =(-hit[kk][4]+hit[gg][4])*TMath::Sqrt( (hit[kk][2]-hit[gg][2])*(hit[kk][2]-hit[gg][2]) + (hit[kk][3]-hit[gg][3])*(hit[kk][3]-hit[gg][3]) );
						same_speed_t[400 + TMath::Nint(15.0/(500.0/max_bin)*(hit[kk][0] - hit[gg][0])/distance) ]++;
					}
					else if(hit[kk][4] + hit[gg][4] == 5 && TMath::Abs(hit[kk][4]-hit[gg][4])==1) 
					{
						 distance =(-hit[kk][4]+hit[gg][4])*TMath::Sqrt( (hit[kk][2]-hit[gg][2])*(hit[kk][2]-hit[gg][2]) + (hit[kk][3]-hit[gg][3])*(hit[kk][3]-hit[gg][3]) );
						same_speed_d1[400 + TMath::Nint(15.0/(500.0/max_bin)*(hit[kk][0] - hit[gg][0])/distance) ]++;	
					}
					else if( TMath::Abs(hit[kk][4]-hit[gg][4])==2)
					{
						distance = (-hit[kk][4]+hit[gg][4])/2.0*TMath::Sqrt( (hit[kk][2]-hit[gg][2])*(hit[kk][2]-hit[gg][2]) + (hit[kk][3]-hit[gg][3])*(hit[kk][3]-hit[gg][3]) );
						same_speed_v[400 + TMath::Nint(15.0/(500.0/max_bin)*(hit[kk][0] - hit[gg][0])/distance) ]++;
					}
					else if(hit[kk][4] + hit[gg][4] == 5 && TMath::Abs(hit[kk][4]-hit[gg][4])==3) 
					{
						distance = (-hit[kk][4]+hit[gg][4])/3.0*TMath::Sqrt( (hit[kk][2]-hit[gg][2])*(hit[kk][2]-hit[gg][2]) + (hit[kk][3]-hit[gg][3])*(hit[kk][3]-hit[gg][3]) );
						same_speed_d2[400 + TMath::Nint(15.0/(500.0/max_bin)*(hit[kk][0] - hit[gg][0])/distance) ]++;
					}
				}
			}
			if(hit[kk][4]==hit[gg][4] && TMath::Abs(hit[kk][1]-hit[gg][1])>=16.0 &&TMath::Abs(hit[kk][1]-hit[gg][1])<20.0 )
			{
				distance = hit[kk][1]-hit[gg][1];
				same_speed_h[400 + TMath::Nint(18.0/(500.0/max_bin)*(hit[kk][0] - hit[gg][0])/distance) ]++;
			}
		}
	}
	return count;

}











double pattern_check(TH2D *trig_pat, double *pat_check_result, int cut_value, std::vector< std::vector< double > > antloc)
{
	int total_hit_count=0;

	int same_speed_t[800];
	int same_speed_v[800];
	int same_speed_d1[800];
	int same_speed_d2[800];
	int same_speed_h[800];
	int c_s[800];
	int c_d[800];
	int speed_cut_value=0.0;


	int speed_t_max=0;
	int speed_v_max=0;
	int speed_h_max=0;
	int speed_d_max1=0;
	int speed_d_max2=0;
	int cs_max=0;
	int cd_max=0;
	
	int temp_speed_t_max=0;
	int temp_speed_v_max=0;
	int temp_speed_h_max=0;
	int temp_speed_d1_max=0;
	int temp_speed_d2_max=0;
	int temp_cs_max=0;
	int temp_cd_max=0;
	
	int max_speed_t_pos=-400;
	int max_speed_v_pos=-400;
	int max_speed_h_pos=-400;
	int max_speed_d1_pos=-400;
	int max_speed_d2_pos=-400;
	int max_cs_pos=-400;
	int max_cd_pos=-400;

	total_hit_count = inter_string_check(trig_pat, &same_speed_t[0], &same_speed_d1[0], &same_speed_d2[0], &same_speed_v[0], &same_speed_h[0],&c_s[0], &c_d[0], antloc);
	
	speed_cut_value =same_speed_d1[416]+c_s[16] +same_speed_d1[415]+c_s[15] +  same_speed_d1[417]+c_s[17] ;

	for(int tt=1; tt<799; tt++)
	{
	temp_speed_t_max = same_speed_t[tt]+same_speed_t[tt-1]+same_speed_t[tt+1];
	temp_speed_v_max = same_speed_v[tt]+same_speed_v[tt-1]+same_speed_v[tt+1];
	temp_speed_h_max = same_speed_h[tt]+same_speed_h[tt-1]+same_speed_h[tt+1];
	temp_speed_d1_max = same_speed_d1[tt]+same_speed_d1[tt-1]+same_speed_d1[tt+1];
	temp_speed_d2_max = same_speed_d2[tt]+same_speed_d2[tt-1]+same_speed_d2[tt+1];
	temp_cs_max = c_s[tt] + c_s[tt-1] + c_s[tt+1];
	temp_cd_max = c_d[tt] + c_d[tt-1] + c_d[tt+1];
	
	if(temp_speed_t_max>speed_t_max ) {speed_t_max = temp_speed_t_max; max_speed_t_pos = tt - 400;}
	if(temp_speed_v_max>speed_v_max) {speed_v_max = temp_speed_v_max; max_speed_v_pos = tt - 400;}
	if(temp_speed_h_max>speed_h_max) {speed_h_max = temp_speed_h_max; max_speed_h_pos = tt - 400;}
	if(temp_speed_d1_max>speed_d_max1) {speed_d_max1 = temp_speed_d1_max; max_speed_d1_pos = tt -400;}
	if(temp_speed_d2_max>speed_d_max2) {speed_d_max2 = temp_speed_d2_max; max_speed_d2_pos = tt - 400;}
	if(temp_cs_max>cs_max){ cs_max= temp_cs_max; max_cs_pos = tt;}
	if(temp_cd_max>cd_max){ cd_max= temp_cd_max; max_cd_pos = tt;}
	}
	double pos_side = TMath::Sqrt( max_speed_t_pos*max_speed_t_pos/15.0/15.0 + max_speed_v_pos*max_speed_v_pos/15.0/15.0 + max_speed_h_pos*max_speed_h_pos/18.0/18.0);
	double pos_diagonal = TMath::Sqrt(max_speed_d1_pos*max_speed_d1_pos/15.0/15.0 + max_speed_d2_pos*max_speed_d2_pos/15.0/15.0 + max_speed_h_pos*max_speed_h_pos/18./18.0);

	pat_check_result[0] = speed_cut_value;		// Time diff complete
	pat_check_result[1] = speed_t_max;
        pat_check_result[2] = speed_v_max;
        pat_check_result[3] = speed_h_max;
        pat_check_result[4] = speed_d_max1;
        pat_check_result[5] = speed_d_max2;
	pat_check_result[6] = pos_side; 
	pat_check_result[7] = pos_diagonal;
	pat_check_result[8] = max_speed_t_pos;
	pat_check_result[9] = max_speed_v_pos;
	pat_check_result[10] = max_speed_h_pos;
	pat_check_result[11] = max_speed_d1_pos;
	pat_check_result[12] = max_speed_d2_pos; 
	pat_check_result[13] = total_hit_count;

	return pos_side;
}





















///int gen_noise_std_dev(UsefulAtriStationEvent *theEvent, double *ch_noise_std_dev, double *ch_noise_average, int sum_time)
int gen_noise_std_dev(std::vector<TGraph *> gr, double *ch_noise_std_dev, double *ch_noise_average, int sum_time)
{
	Double_t *times = 0;
	Double_t *volts = 0;
	double en_average=0;
	double en_var=0;
	double en_std_dev=0;
	double en_sum1=0;
	double en_sum2=0;
	double en_sum3=0;
	double en_sum4=0;
	double en_sq_sum1=0;
	double en_sq_sum2=0;
	double en_sq_sum3=0;
	double en_sq_sum4=0;
	double min_sum1 =0;
	double min_sum2 =0;
	double min_sq_sum1 =0;
	double min_sq_sum2 =0;
	int en_bins = 0;
	int bin;
	for(int rfchan=0;rfchan<16;rfchan++)
	{
//		TGraph *gr_s = theEvent->getGraphFromRFChan(rfchan);
//   		TGraph *gr = FFTtools::getInterpolatedGraph(gr_s,0.5);
		times = gr[rfchan]->GetX();
		volts = gr[rfchan]->GetY();
  		bin =gr[rfchan]->GetN() - 1; 
  		double getx[bin];
                double gety[bin];
  		double prop_energy[bin];
		en_average=0;
		en_var=0;
		en_std_dev=0;
		min_sum1 =0;
		min_sum2 =0;
		min_sq_sum1 =0;
		min_sq_sum2 =0;
		en_sum1=0;
		en_sq_sum1=0;
		en_sum2=0;
		en_sq_sum2=0;
		en_sum3=0;
		en_sq_sum3=0;
		en_sum4=0;
		en_sq_sum4=0;
		en_bins = 0;
		double en_time[bin];
		prop_energy[en_bins]=0;
		for (int l=0; l<bin; l++) 
		{
                      	getx[l] = times[l]; 
		      	gety[l] = volts[l];
			if(l>sum_time)
			{
				for(int ie =0; ie<sum_time;ie++)
				{
		      			prop_energy[en_bins]+=gety[l-sum_time+ie]*gety[l-sum_time+ie];
				}
		     	}
			prop_energy[en_bins] = TMath::Sqrt(prop_energy[en_bins]);
			en_time[en_bins] = getx[l];
				
			if(l<bin/4)en_sum1+=prop_energy[en_bins];
			if(l>bin/4-1 && l<bin/2)en_sum2+=prop_energy[en_bins];
			if(l>bin/2-1 && l<bin/4*3.0)en_sum3+=prop_energy[en_bins];
			if(l>bin/4*3.0-1)en_sum4+=prop_energy[en_bins];

			if(l<bin/4)en_sq_sum1+=prop_energy[en_bins]*prop_energy[en_bins];
			if(l>bin/4-1 && l<bin/2)en_sq_sum2+=prop_energy[en_bins]*prop_energy[en_bins];
			if(l>bin/2-1 && l<bin/4*3.0)en_sq_sum3+=prop_energy[en_bins]*prop_energy[en_bins];
			if(l>bin/4*3.0-1)en_sq_sum4+=prop_energy[en_bins]*prop_energy[en_bins];
			en_bins++;
		  	prop_energy[en_bins] = 0;
                }
//		delete gr;
//		delete gr_s;
		min_sum1 = en_sum1;
		if(en_sum2<min_sum1) {min_sum1 = en_sum2; min_sum2 = en_sum1;}
		else min_sum2 = en_sum2;
		if(en_sum3<min_sum1) {min_sum2 = min_sum1; min_sum1 = en_sum3;}
		else if(en_sum3<min_sum2) {min_sum2 = en_sum3;}
		if(en_sum4<min_sum1) {min_sum2 = min_sum1; min_sum1 = en_sum4;}
		else if(en_sum4<min_sum2) {min_sum2 = en_sum4;}

		min_sq_sum1 = en_sq_sum1;
		if(en_sq_sum2<min_sq_sum1) {min_sq_sum1 = en_sq_sum2; min_sq_sum2 = en_sq_sum1;}
		else min_sq_sum2 = en_sq_sum2;
		if(en_sq_sum3<min_sq_sum1) {min_sq_sum2 = min_sq_sum1; min_sq_sum1 = en_sq_sum3;}
		else if(en_sq_sum3<min_sq_sum2) {min_sq_sum2 = en_sq_sum3;}
		if(en_sq_sum4<min_sq_sum1) {min_sq_sum2 = min_sq_sum1; min_sq_sum1 = en_sq_sum4;}
		else if(en_sq_sum4<min_sq_sum2) {min_sq_sum2 = en_sq_sum4;}

		en_average = (double)(min_sum1 + min_sum2)*2.0/en_bins;
		en_var = 1.0/en_bins*(min_sq_sum1 + min_sq_sum2)*2.0 - en_average*en_average;
		en_std_dev = TMath::Sqrt(en_var);
		ch_noise_std_dev[rfchan] = en_std_dev;
		ch_noise_average[rfchan] = en_average;
  	}
	return en_average;
} 

double gen_trig_histo_single(std::vector<TGraph *> gr, TH2D *trig_pat, double *ch_std_dev, double *ch_average, double th_factor, int sum_time, int stationId)
{
	Double_t *times = 0;
	Double_t *volts = 0;
	int bin = 0;
	int hit_count =0;
	int rfchan =0;
	int en_bins = 0;
	for(int rfchan=0;rfchan<16;rfchan++)
	{//loop channels
//		  TGraph *gr_s = theEvent->getGraphFromRFChan(rfchan);
//   		  TGraph *gr = FFTtools::getInterpolatedGraph(gr_s,0.5);
		  times = gr[rfchan]->GetX();
		  volts = gr[rfchan]->GetY();
		  bin = gr[rfchan]->GetN()-1;
                  double getx[bin];
                  double gety[bin];
  		  double prop_energy[bin];
		  double en_time[bin];
		  en_bins = 0;
		  prop_energy[en_bins] = 0;
                  for (int l=0; l<bin; l++) 
		  {
			getx[l] = times[l];
			gety[l] = volts[l];
			if(l>sum_time){
				for(int ie =0; ie<sum_time;ie++){
		      			prop_energy[en_bins]+=gety[l-sum_time+ie]*gety[l-sum_time+ie];
				}
		     	}
			prop_energy[en_bins] = TMath::Sqrt(prop_energy[en_bins]); //division is wrong, doesn't matter - changed to have the energy outstanding from the waveform in the plot
			en_time[en_bins] = getx[l];
			en_bins++;
		  	prop_energy[en_bins] = 0;
                  }	
//		  delete gr;
//		  delete gr_s;
		  for(int tr=0;tr<en_bins;tr++)
		  {//Fill the trigger pattern histogram
			if(prop_energy[tr]>th_factor*ch_std_dev[rfchan]+ch_average[rfchan] && !(stationId==2 && (rfchan/4==3 && rfchan%4==3)))
			{
				trig_pat->Fill(en_time[tr],rfchan);
				hit_count++;
			}
		  }
	}//end loop channels
  return hit_count;
}

Double_t getQualityParameter(UsefulAtriStationEvent *evPtr)
{

   	double quality = 0;
	double th_factor = 4.0;// atof(argv[2])/10.0;
	int sum_int_time = 8;//atoi(argv[3]);
	int pattern_bin_no = 500/(sum_int_time/2.0);

   	bool cal_check=0;
   	cal_check = evPtr->isCalpulserEvent();
	if(!cal_check)
	{//loop all events
		double ch_noise_std_dev[16];
		double ch_noise_average[16];
		for(int u=0;u<16;u++)
		{
			ch_noise_std_dev[u] = 0;
			ch_noise_average[u] = 0;
		}

		int average_count = 0;

		TGraph *gr_s[16] = {0};
		std::vector<TGraph *> gr;

   		std::vector<double> antl;
   		std::vector<std::vector<double> > ant_loc;

		AraGeomTool *geom = AraGeomTool::Instance();

 		Double_t *antloc=0;
 
   		for(int a=0;a<16;a++)
   		{
			gr_s[a] = evPtr->getGraphFromRFChan(a);
   			gr.push_back( FFTtools::getInterpolatedGraph(gr_s[a],0.5));
			antloc = geom->getStationInfo(evPtr->stationId)->getAntennaInfo(a)->getLocationXYZ();			
			antl.push_back(antloc[0]);
			antl.push_back(antloc[1]);
			antl.push_back(antloc[2]);
//			cout << "Antenna " << a << ", at: " << antl[0] << "   " << antl[1] << "   " << antl[2] << endl;
			ant_loc.push_back(antl);
			antl.clear();
			delete gr_s[a];
   		}

   
		double pat_check_results[20];
		int cut_value = 0;
		int hist_hit_count =0;
		double ch_energy[16] = {0};
     
		TH2D * trig_pat = new TH2D("trig_pat", "The event trigger pattern", pattern_bin_no,0,500, 16,0,16);
//		average_count = gen_noise_std_dev(evPtr, &ch_noise_std_dev[0], &ch_noise_average[0],sum_int_time);
		average_count = gen_noise_std_dev(gr, &ch_noise_std_dev[0], &ch_noise_average[0],sum_int_time);
//		hist_hit_count = gen_trig_histo_single(evPtr, trig_pat, &ch_noise_std_dev[0], &ch_noise_average[0], th_factor,sum_int_time);
		hist_hit_count = gen_trig_histo_single(gr, trig_pat, &ch_noise_std_dev[0], &ch_noise_average[0], th_factor,sum_int_time, evPtr->stationId);
		pattern_check(trig_pat, &pat_check_results[0], cut_value, ant_loc);
		double side_speed = 1.0/(pat_check_results[6]*4.0)*10.0;
		double diagonal_speed = 1.0/(pat_check_results[7]*4.0)*10.0;

		if( (side_speed>1.0 && side_speed<2.5) || (diagonal_speed>1.0 && diagonal_speed<2.5) ) quality = pat_check_results[1] + pat_check_results[2] + pat_check_results[3];
		else quality = 0.0;
//		quality = 0.0;
		for(int i=0;i<16;i++){delete gr[i];}
		delete trig_pat;
	}//end loop all events

  return quality;
}
