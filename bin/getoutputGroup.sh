#!/bin/sh
#
# Usage:
# 
#       getoutputGroup.sh -a hostname
#
# for reading all group values, or
#
#       getoutputGroup.sh hostname U[0-11]
#
# for reading a particular channels group

opt_a=""
i="0"

if [ "$*" == "" ]
then
        echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tgetoutputGroup.sh -a hostname\nOR\n\tgetoutputGroup.sh hostname U[0-11]\n"
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
		snmpget -v 2c -c tof-pub $2 outputGroup.U$i
		i=`expr $i + 1`
	done
	exit
fi

if [ "$*" != "" ]
then
	snmpget -v 2c -c tof-pub $1 outputGroup.$2
fi

