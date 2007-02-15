# $Id$
# $Log$
# Revision 1.2  2003/09/30 20:41:50  geurts
# Added CVS keywords
#
#
gasmon: gasmon.cxx /usr/local/include/pci-das08.h
	gcc -Wall gasmon.cxx -o gasmon -Wl,--rpath -Wl,/usr/local/lib -L/usr/local/lib -lgsl -lgslcblas -lstdc++

clean:
	rm gasmon

cron:
	crontab gasmon.cron

nocron:
	crontab -r
