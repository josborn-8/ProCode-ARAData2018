#!/bin/bash

MOD=$1

STATION=3

cd /data/wipac/ARA/2013/raw/ARA0$STATION-SPS-ARA/

for GZ in /data/wipac/ARA/2013/raw/ARA0$STATION-SPS-ARA/run_00$MOD*
do
   if [[ -d $GZ ]]; then
###	FILE='basename' $GZ
	TEMPRUN=${GZ:46:49}
	RUN=$( printf '%d' $TEMPRUN )
	RUN_FOLDER=$( printf '%6.6d' $RUN )
###	echo $RUN
###	if [[ $RUN -gt 1000 ]]; then
	CHECKFILTERED=/data/wipac/ARA/2013/filtered/full2013Data/ARA0$STATION/root/run$RUN
	FILTEREDFILE=$CHECKFILTERED/event$RUN.root
	GZE=$GZ/event/
	TGZE=/data/wipac/ARA/2013/filtered/unzippedTGZFiles/ARA0$STATION/run_$RUN_FOLDER/event/
###	echo $CHECKFILTERED
	if [[ !(-d $CHECKFILTERED) ]]; then
		GZA=$GZ.tgz
		GZP=$GZ/pedestalValues.run$RUN_FOLDER.dat
		TGZP=/data/wipac/ARA/2013/filtered/unzippedTGZFiles/ARA0$STATION/run_$RUN_FOLDER/pedestalValues.run$RUN_FOLDER.dat
		if [[ -a $GZP ]] || [[ -a $TGZP ]]; then
			echo "Pedestal run"
		else
			FOLDERSIZE=$( du -k -s $GZ | cut -f 1) 
			if [[ -d $GZE ]]; then
				FILESIZE=$( du -k -s $GZE | cut -f 1)
				echo "BAD RUN FOUND! " $RUN ", The Folder " $FOLDERSIZE ", Event size: " $FILESIZE
			else
				echo "BAD RUN FOUND! " $RUN ", The Folder " $FOLDERSIZE ", No event found!"
			fi
			if [[ -a $GZA ]]; then
				GZASIZE=$( du -k -s $GZA | cut -f 1)
				echo "BAD RUN FOUND! " $RUN ", The Folder " $FOLDERSIZE ", Archive size: " $GZASIZE
			else
				echo "BAD RUN FOUND! " $RUN ", The Folder " $FOLDERSIZE ", No archive found!"
			fi
		fi
		echo " "
	else
###		echo "Good run" $RUN
		FILTERSIZE=$( du -k -s $FILTEREDFILE | cut -f 1)
		if [[ -d $TGZE ]]; then
		   TGZESIZE=$( du -k -s $TGZE | cut -f 1)
		   RATIO=$(( TGZSIZE*10 / FILTERSIZE ))
		   if [[ $RATIO -gt 15 ]]; then
			echo "Good run " $RUN " files sizes: " $TGZESIZE " " $FILTERSIZE $RATIO
		   fi
		else
		   FILESIZE1=$( du -k -s $GZE | cut -f 1)
		   RATIO=$(( FILESIZE1*10 / FILTERSIZE ))
		   if [[ $RATIO -gt 15 ]]; then
			echo "Good run " $RUN " files sizes: " $FILESIZE1 " " $FILTERSIZE $RATIO
		   fi
		fi
		
	fi
   fi
done
