#!/bin/bash

MOD=$1

STATION=2
YEAR=2013

#####Actually 1448, but we needed to redo this...
MIN=$(( 2048 + $MOD*5 ))
MAX=$(( 2047 + ($MOD+1)*5 ))

###MIN=$(( 473 + $MOD*5 ))
###MAX=$(( 472 + ($MOD+1)*5 ))

newline=$'\n'

source /home/meures/.bashrc

cd /home/meures/processing/trunk/

rm /data/user/meures/processed/ARA0$STATION-smallRuns$MIN.txt
rm /data/user/meures/processed/ARA0$STATION-pedestalRun$MIN.txt

for RUN in $(seq $MIN $MAX)
  do
	RUN_FOLDER=$( printf '%6.6d' $RUN )
	GZ=/data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER/event
	GZPED=/data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER/pedestalValues.run$RUN_FOLDER.dat
	if [[ -d $GZ ]] && [[ !(-a $GZPED) ]]; then
	    RunSize=$( du -k -s $GZ | cut -f 1)
	    if [[ $RunSize -gt 4000 ]]; then
		echo "Processing run: "	
		echo $GZ
		time bash processFullRun.sh $RUN $STATION $YEAR > processed_Run$RUN_FOLDER.txt
	    else
		echo $RUN $RunSize >> /data/user/meures/processed/ARA0$STATION-smallRuns$MIN.txt
	    fi	
	else
	    echo $RUN $RunSize >> /data/user/meures/processed/ARA0$STATION-pedestalRun$MIN.txt
	fi
done

