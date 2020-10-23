#!/bin/sh
#
# Usage:
#
#       setoutputCurrent.sh hostname U[0-11] [0-25]
#
# for setting a channels output current

if [ "$*" == "" ]
then
        echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tsetoutputCurrent.sh hostname U[0-11] [0-25]\n"
fi

if [ "$*" != "" ]
then
	snmpset -v 2c -c guru $1 outputCurrent.$2 F $3
fi
