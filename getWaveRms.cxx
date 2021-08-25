
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
