#!/bin/sh

for i in 1 3 8 9 0 2 7 12 6 5 11; do
	./getoutputStatus.sh -a tof-lvps"$i"
done

exit
