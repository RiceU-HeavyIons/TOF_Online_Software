#!/bin/sh
# $Id$

./cansend can7 402#2600000300
usleep 200000
./cansend can7 402#2601000400
usleep 200000
./cansend can7 402#2602000100
usleep 200000
./cansend can7 402#2603000200
usleep 200000

./cansend can7 402#2604000700
usleep 200000
./cansend can7 402#2605000800
usleep 200000
./cansend can7 402#2606000500
usleep 200000
./cansend can7 402#2607000600
usleep 200000

./cansend can7 402#2608000b00
usleep 200000
./cansend can7 402#2609000c00
usleep 200000
./cansend can7 402#260a000900
usleep 200000
./cansend can7 402#260b000a00
usleep 200000

./cansend can7 402#260c000f00
usleep 200000
./cansend can7 402#260d007f00
usleep 200000
./cansend can7 402#260e000d00
usleep 200000
./cansend can7 402#260f000e00

exit
