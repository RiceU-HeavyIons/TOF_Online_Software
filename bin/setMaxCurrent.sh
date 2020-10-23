#!/bin/sh
#
# Usage:
#
#       setMaxCurrent.sh hostname U[0-11] [0-25]
#
# for setting a channels max current

if [ "$*" == "" ]
then
        echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tsetMaxCurrent.sh hostname U[0-11] [0-25]\n"
fi

if [ "$*" != "" ]
then
	snmpset -v 2c -c guru $1 outputSupervisionMaxCurrent.$2 F $3
fi
