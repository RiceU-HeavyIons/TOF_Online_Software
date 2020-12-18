#!/bin/sh
# $Id: setTrayID_NW.sh 810 2013-01-07 23:54:57Z jschamba $

# A2, A3, A0, A1
./cansend can101 402#2600001700
usleep 200000
./cansend can101 402#2601001800
usleep 200000
./cansend can101 402#2602001500
usleep 200000
./cansend can101 402#2603007900
usleep 200000

# B2, B3, B0, B1
./cansend can101 402#2604001b00
usleep 200000
./cansend can101 402#2605001c00
usleep 200000
./cansend can101 402#2606001900
usleep 200000
./cansend can101 402#2607001a00
usleep 200000

# C2, C3, C0, C1
./cansend can101 402#2608001f00
usleep 200000
./cansend can101 402#2609002000
usleep 200000
./cansend can101 402#260a001d00
usleep 200000
./cansend can101 402#260b001e00
usleep 200000

# D2, D3, D0, D1
./cansend can101 402#260c002300
usleep 200000
./cansend can101 402#260d002400
usleep 200000
./cansend can101 402#260e002100
usleep 200000
./cansend can101 402#260f002200
usleep 200000

# E2, E3, E0, E1
./cansend can101 402#2610002700
usleep 200000
./cansend can101 402#2611002800
usleep 200000
./cansend can101 402#2612003200
usleep 200000
./cansend can101 402#2613007e00
usleep 200000

# F2, F3, F0, F1
./cansend can101 402#2614002900
usleep 200000
./cansend can101 402#2615002a00
usleep 200000
./cansend can101 402#2616002500
usleep 200000
./cansend can101 402#2617002600
usleep 200000

# G2, G3, G0, G1
./cansend can101 402#2618002e00
usleep 200000
./cansend can101 402#2619002d00
usleep 200000
./cansend can101 402#261a002b00
usleep 200000
./cansend can101 402#261b002c00
usleep 200000

# H2, H3, H0, H1
./cansend can101 402#261c003100
usleep 200000
./cansend can101 402#261d007f00
usleep 200000
./cansend can101 402#261e002f00
usleep 200000
./cansend can101 402#261f003000

usleep 200000

# enable or disable channels
./cansend can101 402#2620001f00 #A
usleep 200000
./cansend can101 402#2621001f00 #B
usleep 200000
./cansend can101 402#2622001f00 #C
usleep 200000
./cansend can101 402#2623001f00 #D
usleep 200000

./cansend can101 402#2624001f00 #E
usleep 200000
./cansend can101 402#2625001f00 #F
usleep 200000
./cansend can101 402#2626001f00 #G
usleep 200000
./cansend can101 402#2627001700 #H
usleep 200000

exit
