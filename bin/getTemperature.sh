#!/bin/sh
#
# Usage:
# 
#       getTemperature.sh -a hostname
#
# for reading all temperature values, or
#
#       getTemperature.sh hostname U[0-11]
#
# for reading a particular channels temperature

opt_a=""
i="0"

if [ "$*" == "" ]
then
        echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tgetTemperature.sh -a hostname\nOR\n\tgetTemperature.sh hostname U[0-11]\n"
fi

while getopts 'a' option
do
        case "$option" in
        "a")    opt_a="1"
                ;;
        esac
done

if [ "$opt_a" != "" ]
then
        while [ $i -le 11 ]
        do
		snmpget -v 2c -c tof-pub $2 outputMeasurementTemperature.U$i
                i=`expr $i + 1`
        done
        exit
fi

if [ "$*" != "" ]
then
	snmpget -v 2c -c tof-pub $1 outputMeasurementTemperature.$2
fi
