#!/bin/sh
#
# Usage:
#
#       setMaxVoltage.sh hostname U[0-11] [0-8]
#
# for setting a channels max voltage

if [ "$*" == "" ]
then
        echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tsetMaxVoltage.sh hostname U[0-11] [0-8]\n"
fi

if [ "$*" != "" ]
then
	snmpset -v 2c -c guru $1 outputSupervisionMaxTerminalVoltage.$2 F $3
fi
