# $Id$
# $Log$
# Revision 1.2  2003/09/30 20:41:50  geurts
# Added CVS keywords
#
#
gasmon: gasmon.cxx /usr/local/include/pci-das08.h
	gcc -Wall gasmon.cxx -o gasmon /usr/lib/libgsl.a

clean:
	rm gasmon

cron:
	crontab gasmon.cron

nocron:
	crontab -r
