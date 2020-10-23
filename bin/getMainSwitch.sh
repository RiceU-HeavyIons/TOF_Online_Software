#!/bin/sh
#
# Usage:
#
#	getMainSwitch.sh hostname
#
# for reading the status of the PL512 switch

if [ "$*" == "" ]
then
        echo -e "April 2 2008, Bertrand H.J. Biritz UCLA\nUsage:\n\n\tgetMainSwitch.sh  hostname\n"
fi

if [ "$*" != "" ]
then
	snmpget -v 2c -c tof-pub $1 sysMainSwitch.0
fi
