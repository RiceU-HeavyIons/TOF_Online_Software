#!/bin/sh
# $Id$

if [ "x$1" = "x" ]; then
    echo $"Usage: $0 <canX>"
    exit 1
fi

./cansend $1 402#9110
./cansend $1 402#9210
./cansend $1 402#9310
./cansend $1 402#9410
./cansend $1 402#9510
./cansend $1 402#9610
./cansend $1 402#9710
./cansend $1 402#9810

exit

