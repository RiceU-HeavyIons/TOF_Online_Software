#!/bin/sh
#
# Usage:
# 
#	getCurrent.sh -a hostname
#
# for reading all current values, or
#
#	getCurrent.sh hostname U[0-11]
#
# for reading a particular channels current

opt_a=""
i="0"

if [ "$*" == "" ]
then
	echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tgetCurrent.sh -a hostname\nOR\n\tgetCurrent.sh hostname U[0-11]\n"
fi

while getopts 'a' option
do
	case "$option" in
	"a")	opt_a="1"
		;;
	esac
done

if [ "$opt_a" != "" ]
then
	while [ $i -le 11 ]
	do
		snmpget -v 2c -c tof-pub $2 outputMeasurementCurrent.U$i
		i=`expr $i + 1`
	done
	exit
fi

if [ "$*" != "" ]
then
	snmpget -v 2c -c tof-pub $1 outputMeasurementCurrent.$2
fi
