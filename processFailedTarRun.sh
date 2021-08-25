#!/bin/bash

RUN=$1
STATION=$2
YEAR=$3

###This script is used to process the runs which were not untarred yet.


source /home/meures/.bashrc

RUN_FOLDER=$( printf '%6.6d' $RUN )


cd /data/wipac/ARA/2013/filtered/unzippedTGZFiles/ARA0$STATION/

##First copy the tgz-file, to get permission to untar it.
###echo "Copying TGZ-File."
###cp /data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER.tgz .

###untar the file.
echo "Extracting TGZ-File."
########tar -xf run_$RUN_FOLDER/.tgz

###Remove the archive
echo "Removing TGZ-File."
pwd
#####rm -f run_$RUN_FOLDER/.tgz

###Go to the processing folder
cd /home/meures/processing/trunk

echo "Start filtering."
bash /home/meures/processing/trunk/testTarFilters.sh $RUN $STATION $YEAR
echo "Rewriting run."
bash /home/meures/processing/trunk/runTarPicker.sh $RUN $STATION $YEAR

cp -rf /data/wipac/ARA/$YEAR/filtered/unzippedTGZFiles/ARA0$STATION/run_$RUN_FOLDER/eventHk /data/wipac/ARA/$YEAR/filtered/unzippedTGZFiles/ARA0$STATION/raw_data/run_$RUN_FOLDER/
cp -rf /data/wipac/ARA/$YEAR/filtered/unzippedTGZFiles/ARA0$STATION/run_$RUN_FOLDER/sensorHk /data/wipac/ARA/$YEAR/filtered/unzippedTGZFiles/ARA0$STATION/raw_data/run_$RUN_FOLDER/
cp -rf /data/wipac/ARA/$YEAR/filtered/unzippedTGZFiles/ARA0$STATION/run_$RUN_FOLDER/monitorHk /data/wipac/ARA/$YEAR/filtered/unzippedTGZFiles/ARA0$STATION/raw_data/run_$RUN_FOLDER/

echo "Processing to root files."
bash /home/meures/processing/trunk/runAtriRunFileMakerTar.sh $RUN $STATION $YEAR

