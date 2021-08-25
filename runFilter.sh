##Run the random filter
BIN=~/repositories/araUW/Software/AraFiltering/build/randomFilter
RAW_DIR=~/ara/data/fromWisconsin2013/ARA02/raw_data/run_001688
OUT_RAW_DIR=~/ara/data/fromWisconsin2013/ARA02/raw_data/filter0
RUN_NUM=1688
MONITOR_FILE=~/ara/data/fromWisconsin2013/ARA02/raw_data/filter0/monitor_1688.root
NUM_EVENTS=200
PED_FILE=/Users/jdavies/ara/data/fromWisconsin2013/ARA02/raw_data/run_001681/pedestalValues.run001681.dat

FILE_LIST=`mktemp event.XXXX`
for file in ${RAW_DIR}/event/ev_*/*; 
do
    if [[ -f $file ]]; then
	echo $file >> ${FILE_LIST}
    fi
done


#Do stuff
rm -rf /Users/jdavies/ara/data/fromWisconsin2013/ARA02/raw_data/filter0/run_001688/

${BIN} ${FILE_LIST} ${OUT_RAW_DIR} ${RUN_NUM} ${MONITOR_FILE} ${NUM_EVENTS} ${PED_FILE}
rm ${FILE_LIST}

