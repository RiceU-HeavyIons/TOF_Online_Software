#!/bin/sh
# $Id$

if [ "x$1" = "x" ]; then
    echo $"Usage: $0 <devID> (dec)"
  exit 1
fi

# issue THUB reconfig command
./pc "m s 0x402 1 0xd $1"
usleep 500000

# turn off all of the Serdes channels on THUB
for ((i=91; i<99; i+=1)) do
    ./pc "m s 0x402 2 0x"$i" 0x1f $1"
    usleep 70000
done

# turn on end-of-run alert messages for THUB NW:
if [ "$1" = "251" ]; then
    ./pc "m s 0x402 2 0xc 0x0 $1"
    usleep 70000
fi


# issue bunch reset
./pc "m s 0x402 2 0x99 0x1 251"

exit

