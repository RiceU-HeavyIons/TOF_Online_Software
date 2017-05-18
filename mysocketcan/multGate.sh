#!/bin/sh

# NW:
./cansend can1 7f2\#0e08"$1"0
sleep 2

# SW:
./cansend can3 7f2\#0e08"$1"0
sleep 2

# NE:
./cansend can2 7f2\#0e08"$1"0
sleep 2

# SE:
./cansend can6 7f2\#0e08"$1"0

exit

