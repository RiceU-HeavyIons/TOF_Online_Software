#!/bin/sh
#
# Usage:
# 
#       getoutputVoltage.sh -a hostname
#
# for reading all output voltage values, or
#
#       getoutputVoltage.sh hostname U[0-11]
#
# for reading a particular channels output voltage

opt_a=""
i="0"

if [ "$*" == "" ]
then
	echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tgetoutputVoltage.sh -a hostname\nOR\n\tgetoutputVoltage.sh hostname U[0-11]\n"
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
		snmpget -v 2c -c tof-pub $2 outputVoltage.U$i
                i=`expr $i + 1`
        done
        exit
fi

if [ "$*" != "" ]
then
	snmpget -v 2c -c tof-pub $1 outputVoltage.$2
fi
