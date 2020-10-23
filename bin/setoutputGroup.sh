#!/bin/sh
#
# Usage:
#
#       setoutputGroup.sh hostname U[0-11] [0-127]
#
# for setting a channels group

if [ "$*" == "" ]
then
        echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tsetoutputGroup.sh hostname U[0-11] [0-127]\n"
fi

if [ "$*" != "" ]
then
	snmpset -v 2c -c guru $1 outputGroup.$2 i $3
fi
