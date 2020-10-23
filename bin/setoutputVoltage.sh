#!/bin/sh
#
# Usage:
#
#       setoutputVoltage.sh hostname U[0-11] [0-8]
#
# for setting a channels voltage

if [ "$*" == "" ]
then
        echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tsetoutputVoltage.sh hostname U[0-11] [0-8]\n"
fi

if [ "$*" != "" ]
then
	snmpset -v 2c -c guru $1 outputVoltage.$2 F $3
fi
