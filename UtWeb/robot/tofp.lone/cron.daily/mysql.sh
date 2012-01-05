#!/bin/sh

DST=/data2/star/tof/backup_mysql
CVR=/homes/tofp/cvsroot
USR=tofp
PASS=t4flight

DB=TOF
mysqldump --user=${USR} --password=${PASS} \
		--add-drop-table --add-locks --all \
		${DB} > ${DST}/${DB}.sql
#		${DB} > ${DST}/${DB}_`date +%Y%m%d_%H%M%S`.sql

DB=toftstand
mysqldump --user=${USR} --password=${PASS} \
		--add-drop-table --add-locks --all \
		${DB} > ${DST}/${DB}.sql
#		${DB} > ${DST}/${DB}_`date +%Y%m%d_%H%M%S`.sql

cd ${DST}
cvs -d ${CVR} commit -m "commit on `date +%Y%m%d_%H%M%S`"

