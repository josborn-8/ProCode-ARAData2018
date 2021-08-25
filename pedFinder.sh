#/bin/bash
if [ "$1" = "" ] || [ "$2" = "" ]
then
   echo "usage: `basename $0` <station> <run>" 1>&2
   exit 1
fi

RUN=$2
STATION=$1

if [ "$STATION" != "ARA02" ] && [ "$STATION" != "ARA03" ]
then
    echo "<station> not ARA02 or ARA03"
    exit 1
fi

BASE_DIR=~/data/${STATION}/2013/raw_data/

if [ ! -d $BASE_DIR/run_`printf %06d $RUN`/ ]
then
    echo "Run $RUN doesn't exist"
    exit 1;
fi

#fixme
N_PED_FILES=`find ${BASE_DIR}/run_* -iname pedestalValues* | wc -l`
#echo N_PED_FILES $N_PED_FILES

if [ $N_PED_FILES == 0 ]
then
    BASE_DIR=~/previous_data/${STATION}/2013/raw_data/
    N_PED_FILES=`find ${BASE_DIR}/run_* -iname pedestalValues* | wc -l`
#    echo N_PED_FILES $N_PED_FILES
    if [ $N_PED_FILES == 0 ]
	then
	exit 1
	fi
fi




TEMP=`ls -d ${BASE_DIR}/run_*/pedestalValues* | head -n 1`
#echo $TEMP
FIRST_PED=${TEMP%*.dat}
#echo $FIRST_PED
FIRST_PED=${FIRST_PED#*pedestalValues.run*}
#echo $FIRST_PED
FIRST_PED=`echo $FIRST_PED | awk '{ print $1 + 0 }'`
#echo $FIRST_PED

TEMP=`ls -d ${BASE_DIR}/run_*/pedestalValues* | tail -n 1`
#echo $TEMP
LAST_PED=${TEMP%*.dat}
##echo $LAST_PED
LAST_PED=${LAST_PED#*pedestalValues.run*}
#echo $LAST_PED
LAST_PED=`echo $LAST_PED | awk '{ print $1 + 0 }'`
#echo $LAST_PED



#echo $RUN $LAST_PED
if [ $RUN -lt $LAST_PED ];
then
    for (( PED = $RUN ; PED <= $LAST_PED ; PED++ ));
      do
      PEDFILE=${BASE_DIR}/run_`printf %06d $PED`/pedestalValues.run`printf %06d $PED`.dat
      python ~/AraFilterTools/trunk/pedChecker.py $PEDFILE
      if [ $? -eq 0 ]; then
	  echo $PEDFILE
	  exit 0;
      fi    
    done
fi

if [ $RUN -gt $FIRST_PED ];
then
    for (( PED = $RUN ; PED >= $FIRST_PED ; PED-- ));
      do
      PEDFILE=${BASE_DIR}/run_`printf %06d $PED`/pedestalValues.run`printf %06d $PED`.dat
      python ~/AraFilterTools/trunk/pedChecker.py $PEDFILE
      if [ $? -eq 0 ]; then
	  echo $PEDFILE
	  exit 0;
      fi    
    done
fi

##Got here if there is no pedestal file in the current ~/data dir
##Try instead ~/previous_di
BASE_DIR=~/previous_data/${STATION}/2013/raw_data/
N_PED_FILES=`find ${BASE_DIR}/run_* -iname pedestalValues* | wc -l`
#    echo N_PED_FILES $N_PED_FILES
if [ $N_PED_FILES == 0 ]
    then
    exit 1
fi

TEMP=`ls -d ${BASE_DIR}/run_*/pedestalValues* | head -n 1`
#echo $TEMP
FIRST_PED=${TEMP%*.dat}
#echo $FIRST_PED
FIRST_PED=${FIRST_PED#*pedestalValues.run*}
#echo $FIRST_PED
FIRST_PED=`echo $FIRST_PED | awk '{ print $1 + 0 }'`
#echo $FIRST_PED

TEMP=`ls -d ${BASE_DIR}/run_*/pedestalValues* | tail -n 1`
#echo $TEMP
LAST_PED=${TEMP%*.dat}
##echo $LAST_PED
LAST_PED=${LAST_PED#*pedestalValues.run*}
#echo $LAST_PED
LAST_PED=`echo $LAST_PED | awk '{ print $1 + 0 }'`
#echo $LAST_PED



#echo $RUN $LAST_PED
if [ $RUN -lt $LAST_PED ];
then
    for (( PED = $RUN ; PED <= $LAST_PED ; PED++ ));
      do
      PEDFILE=${BASE_DIR}/run_`printf %06d $PED`/pedestalValues.run`printf %06d $PED`.dat
      python ~/AraFilterTools/trunk/pedChecker.py $PEDFILE
      if [ $? -eq 0 ]; then
	  echo $PEDFILE
	  exit 0;
      fi    
    done
fi

if [ $RUN -gt $FIRST_PED ];
then
    for (( PED = $RUN ; PED >= $FIRST_PED ; PED-- ));
      do
      PEDFILE=${BASE_DIR}/run_`printf %06d $PED`/pedestalValues.run`printf %06d $PED`.dat
      python ~/AraFilterTools/trunk/pedChecker.py $PEDFILE
      if [ $? -eq 0 ]; then
	  echo $PEDFILE
	  exit 0;
      fi    
    done
fi
