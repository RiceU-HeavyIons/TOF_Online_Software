#!/bin/sh
# $Id$

# NW:
./cansend can1 7f2\#0e0980
sleep 3

# SW:
./cansend can3 7f2\#0e0980
sleep 3

# NE:
./cansend can2 7f2\#0e0980
sleep 3

# SE:
./cansend can6 7f2\#0e0980

exit

