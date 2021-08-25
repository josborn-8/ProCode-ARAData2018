#!/bin/bash

MOD=$1

STATION=2
YEAR=2013

MIN=$(( 2460 + $MOD*3 ))
MAX=$(( 2459 + ($MOD+1)*3 ))


newline=$'\n'

source /home/meures/.bashrc

cd /home/meures/processing/trunk/

rm /data/user/meures/processed/ARA0$STATION-smallRuns$MIN.txt
rm /data/user/meures/processed/ARA0$STATION-pedestalRun$MIN.txt




for RUN in $(seq $MIN $MAX)
  do
	RUN_FOLDER=$( printf '%6.6d' $RUN )
	GZ=/data/wipac/ARA/$YEAR/filtered/unzippedTGZFiles/ARA0$STATION/run_$RUN_FOLDER
	if [[ -d $GZ ]]; then
	  GZU=$GZ/.tgz
	  RunSize=$( du -k -s $GZ | cut -f 1)
###	  RunSizeU=$( du -k -s $GZU | cut -f 1)
	    if [[ $RunSize -gt 10000 ]] && [[ -a $GZU ]]; then
		echo "Processing run: "	
		echo $GZ
		time bash processFailedTarRun.sh $RUN $STATION $YEAR > processed_Run$RUN_FOLDER.txt
	    else
		echo $RUN $RunSize >> /data/user/meures/processed/ARA0$STATION-smallRuns$MIN.txt
	    fi	
	else
	    echo $RUN $RunSize >> /data/user/meures/processed/ARA0$STATION-pedestalRun$MIN.txt
	fi
done

