#!/bin/bash

MOD=$1

STATION=3
YEAR=2013

MIN=$(( 960 + $MOD*5 ))
MAX=$(( 959 + ($MOD+1)*5 ))


newline=$'\n'

source /home/meures/.bashrc

cd /home/meures/processing/trunk/

rm /data/user/meures/processed/ARA0$STATION-smallRuns$MIN.txt
rm /data/user/meures/processed/ARA0$STATION-pedestalRun$MIN.txt




for RUN in $(seq $MIN $MAX)
  do
	RUN_FOLDER=$( printf '%6.6d' $RUN )
	GZ=/data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER.tgz
	if [[ -a $GZ ]]; then
	  GZU=/data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER
	  RunSize=$( du -k -s /data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER.tgz | cut -f 1)
	  RunSizeU=$( du -k -s /data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER | cut -f 1)
	  if [[ -a $GZ ]]; then
	    if [[ $RunSize -gt 1400 ]] && [[ $RunSizeU -lt 100000 ]]; then
		echo "Processing run: "	
		echo $GZ
		time bash processTarRun.sh $RUN $STATION $YEAR > processed_Run$RUN_FOLDER.txt
	    else
		echo $RUN $RunSize >> /data/user/meures/processed/ARA0$STATION-smallRuns$MIN.txt
	    fi	
	  else
	    echo $RUN $RunSize >> /data/user/meures/processed/ARA0$STATION-pedestalRun$MIN.txt
	  fi
	fi
done

