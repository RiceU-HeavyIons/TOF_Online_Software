#!/bin/sh
#
# Usage:
#
#       setMinSenseVoltage.sh hostname U[0-11] [0-8]
#
# for setting a channels minimum sense voltage

if [ "$*" == "" ]
then
        echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tsetMinSenseVoltage.sh hostname U[0-11] [0-8]\n"
fi

if [ "$*" != "" ]
then
	snmpset -v 2c -c guru $1 outputSupervisionMinSenseVoltage.$2 F $3
fi
