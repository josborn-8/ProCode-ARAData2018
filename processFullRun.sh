#!/bin/bash

RUN=$1
STATION=$2
YEAR=$3


source /home/meures/.bashrc
echo "Processing to root files."
bash /home/meures/processing/trunk/runAtriFullRunFileMaker.sh $RUN $STATION $YEAR

