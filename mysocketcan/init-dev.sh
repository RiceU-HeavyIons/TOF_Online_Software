#!/bin/bash

if [ "$1" == "" ]
then
  DEV="can100"
else
  DEV="$1"
fi

ip link set $DEV type can bitrate 1000000
ifconfig $DEV up
