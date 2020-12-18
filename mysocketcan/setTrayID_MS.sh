#!/bin/sh
# $Id: setTrayID_MS.sh 881 2013-07-08 21:53:01Z jschamba $

./cansend can105 402#2600001200
usleep 200000
./cansend can105 402#2601001300
usleep 200000
./cansend can105 402#2602001000
usleep 200000
./cansend can105 402#2603001100
usleep 200000

./cansend can105 402#2604001600
usleep 200000
./cansend can105 402#2605001700
usleep 200000
./cansend can105 402#2606001400
usleep 200000
./cansend can105 402#2607001500
usleep 200000

./cansend can105 402#2608001a00
usleep 200000
./cansend can105 402#2609001b00
usleep 200000
./cansend can105 402#260a001800
usleep 200000
./cansend can105 402#260b001900
usleep 200000

./cansend can105 402#260c001e00
usleep 200000
./cansend can105 402#260d001f00
usleep 200000
./cansend can105 402#260e001c00
usleep 200000
./cansend can105 402#260f001d00

exit
