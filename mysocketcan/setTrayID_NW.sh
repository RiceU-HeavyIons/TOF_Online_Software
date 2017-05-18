#!/bin/sh
# $Id: setTrayID_NW.sh 810 2013-01-07 23:54:57Z jschamba $

./cansend can1 402#2600001700
usleep 200000
./cansend can1 402#2601001800
usleep 200000
./cansend can1 402#2602001500
usleep 200000
./cansend can1 402#2603001600
usleep 200000

./cansend can1 402#2604001b00
usleep 200000
./cansend can1 402#2605001c00
usleep 200000
./cansend can1 402#2606001900
usleep 200000
./cansend can1 402#2607001a00
usleep 200000

./cansend can1 402#2608001f00
usleep 200000
./cansend can1 402#2609002000
usleep 200000
./cansend can1 402#260a001d00
usleep 200000
./cansend can1 402#260b001e00
usleep 200000

./cansend can1 402#260c002300
usleep 200000
./cansend can1 402#260d002400
usleep 200000
./cansend can1 402#260e002100
usleep 200000
./cansend can1 402#260f002200
usleep 200000

./cansend can1 402#2610002700
usleep 200000
./cansend can1 402#2611002800
usleep 200000
./cansend can1 402#2612002500
usleep 200000
./cansend can1 402#2613002600
usleep 200000

./cansend can1 402#2614002b00
usleep 200000
./cansend can1 402#2615002c00
usleep 200000
./cansend can1 402#2616002900
usleep 200000
./cansend can1 402#2617002a00
usleep 200000

./cansend can1 402#2618002f00
usleep 200000
./cansend can1 402#2619003000
usleep 200000
./cansend can1 402#261a002d00
usleep 200000
./cansend can1 402#261b002e00
usleep 200000

./cansend can1 402#261c007900
usleep 200000
./cansend can1 402#261d007f00
usleep 200000
./cansend can1 402#261e003100
usleep 200000
./cansend can1 402#261f003200

exit
