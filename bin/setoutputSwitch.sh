#!/bin/sh
#
# Usage:
#
#       setoutputSwitch.sh hostname U[0-11] [0-1]
#
# for turning on/off a channel

if [ "$*" == "" ]
then
        echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tsetoutputSwitch.sh hostname U[0-11] [0-1]\n"
fi

if [ "$*" != "" ]
then
	snmpset -v 2c -c guru $1 outputSwitch.$2 i $3
fi
