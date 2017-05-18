#!/bin/sh
# $Id: serdesOn.sh 797 2012-11-26 16:04:07Z jschamba $

if [ "x$1" = "x" ]; then
    echo $"Usage: $0 <canX>"
    exit 1
fi

./cansend $1 402#911f
./cansend $1 402#921f
./cansend $1 402#931f
./cansend $1 402#941f
./cansend $1 402#951f
./cansend $1 402#961f
./cansend $1 402#971f
./cansend $1 402#9813

exit

