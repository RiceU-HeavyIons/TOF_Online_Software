#!/bin/sh
# $Id: testpatternOn.sh 809 2013-01-07 23:53:36Z jschamba $

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

