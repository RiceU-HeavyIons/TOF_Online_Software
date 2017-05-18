#!/bin/sh
# $Id$

if [ "x$1" = "x" ]; then
    echo $"Usage: $0 <canX>"
    exit 1
fi

./cansend $1 402#0d
usleep 400000
./cansend $1 7f2#60

exit

