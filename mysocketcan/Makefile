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

PROGRAMS = candump cangen cansend \
		getFWIDs getTHUBFWIDs getChecksum x_getChecksum \
		x_config_pm x_config x_config_scripted \
		MCU2 x_MCU2 eeprom2 x_eeprom2 \
		progMCU progPLD \
		x_setThreshold showSerdes x_enableChannel \
		x_readHPTDCconfig

all: $(PROGRAMS)

clean:
	rm -f $(PROGRAMS) *.o

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(PROGRAMS) $(DESTDIR)$(PREFIX)/bin

distclean:
	rm -f $(PROGRAMS) *.o *~

cansend.o:	lib.h
cangen.o:	lib.h
candump.o:	lib.h
canplayer.o:	lib.h
canlogserver.o:	lib.h
canbusload.o:	lib.h
log2long.o:	lib.h
log2asc.o:	lib.h
asc2log.o:	lib.h

getFWIDs.o:		can_utils.h
eeprom2.o:		can_utils.h
x_eeprom2.o:		can_utils.h
MCU2.o:			can_utils.h
x_MCU2.o:		can_utils.h
getTHUBFWIDs.o:		can_utils.h
getChecksum.o:		can_utils.h
x_getChecksum.o:	can_utils.h
x_config.o:		can_utils.h
x_config_pm.o:		can_utils.h
x_config_scripted.o:	can_utils.h
progMCU.o:		can_utils.h
progPLD.o:		can_utils.h
x_setThreshold.o:	can_utils.h
x_enableChannel.o:	can_utils.h
showSerdes.o:		can_utils.h
x_readHPTDCconfig.o:	can_utils.h

cansend:	cansend.o	lib.o
cangen:		cangen.o	lib.o
candump:	candump.o	lib.o
canplayer:	canplayer.o	lib.o
canlogserver:	canlogserver.o	lib.o
log2long:	log2long.o	lib.o
log2asc:	log2asc.o	lib.o
asc2log:	asc2log.o	lib.o

MCU2:		MCU2.o can_utils.o
	$(CXX) -Wall $^ -o $@

x_MCU2:		x_MCU2.o can_utils.o
	$(CXX) -Wall $^ -o $@

eeprom2:	eeprom2.o can_utils.o
	$(CXX) -Wall $^ -o $@

x_eeprom2:	x_eeprom2.o can_utils.o
	$(CXX) -Wall $^ -o $@

progMCU:	progMCU.o can_utils.o
	$(CXX) -Wall $^ -o $@

progPLD:	progPLD.o can_utils.o
	$(CXX) -Wall $^ -o $@

getFWIDs:	getFWIDs.o can_utils.o
	$(CXX) -Wall $^ -o $@

x_config:	x_config.o can_utils.o
	$(CXX) -Wall $^ -o $@

x_config_pm:	x_config_pm.o can_utils.o
	$(CXX) -Wall $^ -o $@

x_config_scripted:	x_config_scripted.o can_utils.o
	$(CXX) -Wall $^ -o $@

getChecksum:	getChecksum.o can_utils.o
	$(CXX) -Wall $^ -o $@

x_getChecksum:	x_getChecksum.o can_utils.o
	$(CXX) -Wall $^ -o $@

getTHUBFWIDs:	getTHUBFWIDs.o can_utils.o
	$(CXX) -Wall $^ -o $@

x_setThreshold:	x_setThreshold.o can_utils.o
	$(CXX) -Wall $^ -o $@

showSerdes:	showSerdes.o can_utils.o
	$(CXX) -Wall $^ -o $@

x_enableChannel:	x_enableChannel.o can_utils.o
	$(CXX) -Wall $^ -o $@

x_readHPTDCconfig:	x_readHPTDCconfig.o can_utils.o
	$(CXX) -Wall $^ -o $@

