#!/bin/sh
#
# Usage:
#
#       setGroupSwitch.sh hostname [0-127] [0-1]
#
# for setting a channels group

if [ "$*" == "" ]
then
        echo -e "January 10 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tsetGroupSwitch.sh hostname [0-127] [0-1]\n"
fi

if [ "$*" != "" ]
then
	snmpset -v 2c -c guru $1 groupsSwitch.$2 i $3
fi
