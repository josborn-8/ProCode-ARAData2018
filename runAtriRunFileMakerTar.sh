#/bin/bash
if [ "$1" = "" ]
then
   echo "usage: `basename $0` <run num> <station Number> <year>" 1>&2
   exit 1
fi

RUN_NUMBER=$1
STATION=$2
YEAR=$3
RUN_FOLDER=$( printf '%6.6d' $RUN_NUMBER )

#Only need to edit these two lines to point to the local directories 
#RAW_BASE_DIR=~/ara/data/testing_for_trunk/event/ARA2
RAW_BASE_DIR=/data/wipac/ARA/$YEAR/filtered/unzippedTGZFiles/ARA0$STATION/raw_data/
ROOT_BASE_DIR=/data/wipac/ARA/$YEAR/filtered/unzippedTGZFiles/ARA0$STATION/root/
###RAW_BASE_DIR=/data/user/meures/testData/ARA02/2014/raw_data/
###ROOT_BASE_DIR=/data/user/meures/testData/ARA02/2014/root
#RAW_BASE_DIR=/tmp/fakeData
#ROOT_BASE_DIR=/tmp/fakeRoot
##RAW_BASE_DIR=/home/meures/AraCalibration/uhen/ara/data/calibration/ARA01/raw_data
##ROOT_BASE_DIR=/home/meures/AraCalibration/uhen/ara/data/calibration/ARA01/root

RUN_NUM=$RUN_NUMBER
RUN_WITH_ZEROES=`printf %06d $RUN_NUM`
echo $RUN_NUM $RUN_WITH_ZEROES

RAW_DIR=${RAW_BASE_DIR}/run_${RUN_WITH_ZEROES}
ROOT_DIR=${ROOT_BASE_DIR}/run${RUN_NUM}
EVENT_FILE=${ROOT_DIR}/event${RUN_NUM}.root
SENSOR_HK_FILE=${ROOT_DIR}/sensorHk${RUN_NUM}.root
EVENT_HK_FILE=${ROOT_DIR}/eventHk${RUN_NUM}.root
#exit 1
#echo ${RAW_DIR}

if [[ -d $ROOT_DIR ]]; then
    echo "Output dir exists"
else
    mkdir ${ROOT_DIR}
fi

echo "Starting Event File"
EVENT_FILE_LIST=`mktemp event.XXXX`
for file in ${RAW_DIR}/event/ev_*/*; 
do
  if [[ -f $file ]]; then
      echo $file >> ${EVENT_FILE_LIST}
#      echo `dirname $file`;
  fi
done

if  test `cat ${EVENT_FILE_LIST} | wc -l` -gt 0 ; then
    ${ARA_UTIL_INSTALL_DIR}/bin/makeAtriEventTree ${EVENT_FILE_LIST} ${EVENT_FILE} ${RUN_NUM}
    #cat ${EVENT_FILE_LIST}
    rm ${EVENT_FILE_LIST}
    echo "Done Event File"
else
    rm ${EVENT_FILE_LIST}
    echo "No event files"
fi


echo "Starting Sensor Hk File"
SENSOR_HK_FILE_LIST=`mktemp sensor.XXXX`
for file in ${RAW_DIR}/sensorHk/sensorHk_*/*; 
  do
  if [[ -f $file ]]; then
      echo $file >> ${SENSOR_HK_FILE_LIST}
#      echo `dirname $file`;
  fi
done

if  test `cat ${SENSOR_HK_FILE_LIST} | wc -l` -gt 0 ; then
    ${ARA_UTIL_INSTALL_DIR}/bin/makeAtriSensorHkTree ${SENSOR_HK_FILE_LIST} ${SENSOR_HK_FILE} ${RUN_NUM}
#    cat ${SENSOR_HK_FILE_LIST}
    rm ${SENSOR_HK_FILE_LIST}
    echo "Done Sensor Hk File"
else
    rm ${SENSOR_HK_FILE_LIST}
    echo "No sensor hk files"
fi



echo "Starting Event Hk File"
EVENT_HK_FILE_LIST=`mktemp event.XXXX`
for file in ${RAW_DIR}/eventHk/eventHk_*/*; 
  do
  if [[ -f $file ]]; then
      echo $file >> ${EVENT_HK_FILE_LIST}
#      echo `dirname $file`;
  fi
done

if  test `cat ${EVENT_HK_FILE_LIST} | wc -l` -gt 0 ; then
    ${ARA_UTIL_INSTALL_DIR}/bin/makeAtriEventHkTree ${EVENT_HK_FILE_LIST} ${EVENT_HK_FILE} ${RUN_NUM}
#    cat ${EVENT_HK_FILE_LIST}
    rm ${EVENT_HK_FILE_LIST}
    echo "Done Event Hk File"
else
    rm ${EVENT_HK_FILE_LIST}
    echo "No event hk files"
fi


##rm -rf $RAW_DIR
