#!/bin/sh
# $Id: testpatternOn.sh 809 2013-01-07 23:53:36Z jschamba $

# NW:
./cansend can101 7f2\#0e0980
sleep 3

# SW:
./cansend can103 7f2\#0e0980
sleep 3

# NE:
./cansend can102 7f2\#0e0980
sleep 3

# SE:
./cansend can106 7f2\#0e0980

exit
