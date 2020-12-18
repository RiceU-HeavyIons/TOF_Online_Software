#!/bin/sh

# NW:
./cansend can101 7f2\#0e08"$1"0
sleep 2

# SW:
./cansend can103 7f2\#0e08"$1"0
sleep 2

# NE:
./cansend can102 7f2\#0e08"$1"0
sleep 2

# SE:
./cansend can106 7f2\#0e08"$1"0

exit
