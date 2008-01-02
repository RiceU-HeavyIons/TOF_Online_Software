# $Id$

CXXFLAGS += -Wall

DIST_FILES = pcanloop.cc pc.c p_config.cc can_utils.cc can_utils.h \
		Makefile xp_config.cc xp_config_scripted p_eeprom2.cc \
		xp_eeprom2.cc pcan_find.cc getStatus.cc xgetStatus.cc \
		p_progPLD.cc p_progMCU.cc MCU2.cc xMCU2.cc xenableChannel.cc \
		enableChannel.cc xsetThreshold.cc setThreshold.cc \
		cfig_test014*.txt x*.sh *script.txt

# Targets:

all:		pcanloop pc p_config pcan_find setThreshold \
		getStatus enableChannel \
		change_mcu_program analyze_noise clientpc MCU2 \
		p_progPLD p_progMCU p_eeprom2 \
		xMCU2 xp_eeprom2 xp_config_reset xp_config_scripted \
		xgetStatus xp_config xsetThreshold xenableChannel

pcanloop:	pcanloop.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pc:	pc.c
	$(CC) -Wall $^ -o $@

p_config:	p_config.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_config:	xp_config.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_config_scripted:	xp_config_scripted.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_config_reset:	xp_config_reset.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

p_eeprom2:	p_eeprom2.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_eeprom2:	xp_eeprom2.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

change_mcu_program:	change_mcu_program.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pcan_find:	pcan_find.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pcanloop2:	pcanloop2.cc
	$(CXX) -Wall $^ -o $@ -lpcan

clientpc:	clientpc.c
	$(CXX) -Wall $^ -o $@ -lpcan

getStatus:	getStatus.cc
	$(CXX) -Wall $^ -o $@ -lpcan

xgetStatus:	xgetStatus.cc
	$(CXX) -Wall $^ -o $@ -lpcan

p_progPLD:	p_progPLD.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

p_progMCU:	p_progMCU.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

MCU2:		MCU2.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xMCU2:		xMCU2.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

clean:
	rm -f *.o pcanloop pc p_config change_mcu_program pcan_find setThreshold \
		p_config_run5 analyze_noise clientpc MCU2 \
		p_progPLD p_progMCU can_utils.o getStatus p_eeprom2 enableChannel \
		xgetStatus xp_config xsetThreshold xenableChannel \
		xMCU2 xp_eeprom2 xp_config_reset

backup:
	rm -f pcan.tgz; \
	tar -zcf pcan.tgz $(DIST_FILES)

