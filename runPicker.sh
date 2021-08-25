#!/bin/bash


##Run the picker
RUN_NUMBER=$1
STATION=$2
YEAR=$3

source /home/meures/.bashrc

BIN=~/processing/trunk/build/AraEventPicker
##RAW_DIR=~/ara/data/fromWisconsin2013/ARA02/raw_data/filter2/run_001688
RUN_FOLDER=$( printf '%6.6d' $RUN_NUMBER )
RAW_DIR=/data/wipac/ARA/$YEAR/filtered/burnSample1in10/ARA0$STATION/raw_data_tagged/run_$RUN_FOLDER
OUT_RAW_DIR=/data/wipac/ARA/$YEAR/filtered/burnSample1in10/ARA0$STATION/raw_data
###OUT_RAW_DIR=~/ara/data/fromWisconsin2013/ARA02/raw_data/filter3
RUN_NUM=$RUN_NUMBER

FILE_LIST=`mktemp event$RUN_NUMBER.XXXX`
for file in ${RAW_DIR}/event/ev_*/*; 
do
    if [[ -f $file ]]; then
	echo $file >> ${FILE_LIST}
    fi
done

#Do stuff
##rm -rf /Users/jdavies/ara/data/fromWisconsin2013/ARA02/raw_data/filter3/run_001688/
${BIN} ${FILE_LIST} ${OUT_RAW_DIR} ${RUN_NUM} 
rm ${FILE_LIST}

rm -rf $RAW_DIR
