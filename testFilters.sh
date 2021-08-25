#!/bin/bash
RUN_NUMBER=$1
STATION=$2
YEAR=$3
RUN_FOLDER=$( printf '%6.6d' $RUN_NUMBER )


source /home/meures/.bashrc

#filter2
###BIN=~/code_svn/AraFiltering/trunk/build/nchnlFilter
BIN=~/processing/trunk/build/randomFilter
RAW_DIR=/data/wipac/ARA/$YEAR/raw/ARA0$STATION-SPS-ARA/run_$RUN_FOLDER
OUT_RAW_DIR=/data/wipac/ARA/$YEAR/filtered/burnSample1in10/ARA0$STATION/raw_data_tagged
###RAW_DIR=/data/exp/ARA/2013/filtered/L0/ARA03/0301/run522
###OUT_RAW_DIR=/data/user/meures/processed
RUN_NUM=$RUN_NUMBER
MONITOR_FILE=/data/user/meures/processed/monitor_$RUN_NUM.root
NUM_EVENTS=100
PED_FILE=~/data_files/ARA0$STATION/pedestals/ARA0$STATION/$YEAR/raw_data/run_001502/pedestalValues.run001502.dat

FILE_LIST=`mktemp event$RUN_NUMBER.XXXX`
for file in ${RAW_DIR}/event/ev_*/*; 
do
    if [[ -f $file ]]; then
	echo $file >> ${FILE_LIST}
    fi
done

#Do stuff
##rm -rf ~/testRawData/filter0/run_000522/

${BIN} ${FILE_LIST} ${OUT_RAW_DIR} ${RUN_NUM} ${MONITOR_FILE} ${NUM_EVENTS} ${PED_FILE}
rm ${FILE_LIST}

