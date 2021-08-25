#!/bin/bash





cd /data/wipac/ARA/2013/raw/ARA02-SPS-ARA/

for GZ in /data/wipac/ARA/2013/raw/ARA02-SPS-ARA/run_00*
do
	GZE=$GZ/event/
	if [[ -d $GZE ]]; then
		FOLDERSIZE=$( du -k -s $GZ | cut -f 1) 
		FILESIZE=$( du -k -s $GZE | cut -f 1)
		if [[ $FOLDERSIZE -gt 20000 ]] && [[ $FILESIZE -lt 10000 ]]; then
			echo "ALERT: Bad run " $GZ
		fi
	fi
done
