#
#  $Id$
#

DESTDIR =
PREFIX = /usr/local

MAKEFLAGS = -k

CXXFLAGS += -O2 -Wall

#CFLAGS    = -O2 -Wall -Wno-parentheses -Iinclude \

CFLAGS    = -O2 -Wall -Wno-parentheses \
	    -fno-strict-aliasing \
	    -DSO_RXQ_OVFL=40 \
	    -DPF_CAN=29 \
	    -DAF_CAN=PF_CAN

PROGRAMS = vcanAgent

all: $(PROGRAMS)

clean:
	rm -f $(PROGRAMS) *.o

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(PROGRAMS) $(DESTDIR)$(PREFIX)/bin

distclean:
	rm -f $(PROGRAMS) *.o *~


vcanAgent:	vcanAgent.o
	$(CXX) -Wall $^ -o $@

