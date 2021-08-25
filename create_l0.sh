STATION=$1
RUN=$2
YEAR=2013
###DATA_DIR=/data/exp/ARA/$YEAR/filtered/$STATION-pedestals/$DAY
for DATA_DIR in /data/user/meures/processing/raw_data/*
do
##DATA_DIR=/data/exp/ARA/$YEAR/calibration/ARA03-SPS-ARA/0617/
##DATA_DIR=/data/exp/ARA/$YEAR/unbiased/$STATION-SPS-ARA/$MONTH*
##DATA_DIR=/data/exp/ARA/2014/filtered/$STATION-SPS-ARA/$MONTH*
##  L0_OUT_DIR=/home/meures/data_files/$STATION/$YEAR/raw_data
###L0_OUT_DIR=/home/meures/data_files/ARA03
  L0_OUT_DIR=/data/user/meures/processing/raw_data
  mkdir -p $L0_OUT_DIR

  cd $L0_OUT_DIR

  ####for GZ in $DATA_DIR/SPS-ARA-FILTERED-run-*.tar.gz
###  for GZ in $DATA_DIR/SPS-ARA-CALIBRATION-run*
  for GZ in $DATA_DIR/SPS-ARA-FILTERED-run*
  do
    echo looking at $GZ
    FILE=`basename $GZ`
###   RUN=${FILE:21:6}
    RUN=${FILE:24:6}
    RUNNO=${FILE:27:3}
##   if [ ${FILE:23:1} -eq 0 ]; then
 ##    RUNNO=${FILE:24:3}
##   else
##     RUNNO=${FILE:23:4}
##   fi
   RUN_NAME=${FILE/.tar.gz/}
   tar -xzf $GZ
   tar -xf  $RUN_NAME.dat.tar
   rm $RUN_NAME.dat.tar
   rm $RUN_NAME.meta.xml
   find run_$RUN/. -iname "ev_*.dat" > run-$RUN.filelist
##   cd /home/meures/utilities/Atri
##   ./runAtriRunFileMaker.sh $RUNNO $STATION
##   cd $L0_OUT_DIR
##   rm -r run_$RUN
   rm run-$RUN.filelist
  done
done
