#!/bin/bash

RUN=$1
STATION=$2
YEAR=$3


source /home/meures/.bashrc
echo "Start filtering."
bash /home/meures/processing/trunk/testFilters.sh $RUN $STATION $YEAR
echo "Rewriting run."
bash /home/meures/processing/trunk/runPicker.sh $RUN $STATION $YEAR

RUN_FOLDER=$( printf '%6.6d' $RUN )
cp -rf /data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER/eventHk /data/wipac/ARA/$YEAR/filtered/burnSample1in10/ARA0$STATION/raw_data/run_$RUN_FOLDER/
cp -rf /data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER/sensorHk /data/wipac/ARA/$YEAR/filtered/burnSample1in10/ARA0$STATION/raw_data/run_$RUN_FOLDER/
cp -rf /data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER/monitorHk /data/wipac/ARA/$YEAR/filtered/burnSample1in10/ARA0$STATION/raw_data/run_$RUN_FOLDER/

echo "Processing to root files."
bash /home/meures/processing/trunk/runAtriRunFileMaker.sh $RUN $STATION $YEAR

