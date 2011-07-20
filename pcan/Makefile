# $Id$

CXXFLAGS += -Wall

DIST_FILES = pcanloop.cc pc.c can_utils.cc can_utils.h \
		Makefile xp_config.cc xp_config_scripted.cc \
		pcan_find.cc xgetStatus.cc showSerdes.cc \
		xenableChannel.cc xsetThreshold.cc \
		getFWIDs.cc getTHUBFWIDS.cc getChecksum*.cc x_getChecksum*.cc \
		cfig_test014*.txt cfig_test016*.txt cfig_test018*.txt \
		initTHUB*.sh turnonscript*.sh

# Targets:

all:		pcanloop pc pcan_find showSerdes \
		xgetStatus xp_config xsetThreshold xenableChannel \
		xp_config_scripted xp_config_reset p_config \
		x_getChecksum_l getChecksum_l xp_config_scripted_l \
		setThreshold getStatus enableChannel x_getChecksum getChecksum\
		MCUchecksum THUBchecksum getFWIDs getTHUBFWIDs \
		change_mcu_program analyze_noise clientpc MCU2 \
		p_progPLD p_progMCU p_eeprom2 xp_config_pm \
		p_eeprom2_l p_progPLD_l p_progMCU_l MCU2_l \
		xMCU2 xp_eeprom2 xMCU2_withOffset xMCU2_noIVT \
		xp_eeprom2_l xMCU2_l xp_config_pm_l \
		x_readHPTDCconfig_l


pcanloop:	pcanloop.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pc:	pc.c
	$(CC) -Wall $^ -o $@

p_config:	p_config.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_config:	xp_config.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_config_pm:	xp_config_pm.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_config_scripted:	xp_config_scripted.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_config_pm_l:	xp_config_pm_l.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_config_scripted_l:	xp_config_scripted_l.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_config_reset:	xp_config_reset.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

p_eeprom2:	p_eeprom2.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

p_eeprom2_l:	p_eeprom2_l.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_eeprom2:	xp_eeprom2.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xp_eeprom2_l:	xp_eeprom2_l.o can_utils.o
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

xgetStatus:	xgetStatus.cc can_utils.o
	$(CXX) -Wall $^ -o $@ -lpcan

p_progPLD:	p_progPLD.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

p_progPLD_l:	p_progPLD_l.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

p_progMCU:	p_progMCU.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

p_progMCU_l:	p_progMCU_l.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

MCU2:		MCU2.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

MCU2_l:		MCU2_l.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xMCU2:		xMCU2.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xMCU2_l:	xMCU2_l.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xMCU2_noIVT:	xMCU2_noIVT.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

xMCU2_withOffset:	xMCU2_withOffset.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

x_getChecksum:	x_getChecksum.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

x_getChecksum_l:	x_getChecksum_l.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

getChecksum:	getChecksum.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

getChecksum_l:	getChecksum_l.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

getTHUBFWIDs:		getTHUBFWIDs.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

showSerdes:		showSerdes.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

getFWIDs:		getFWIDs.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

x_readHPTDCconfig_l:	x_readHPTDCconfig_l.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

sendcmd:	sendcmd.cc
	$(CXX) -Wall $^ -o $@ -lpcan

checkCan:	checkCan.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

clean:
	rm -f *.o pcanloop pc p_config change_mcu_program pcan_find setThreshold \
		p_config_run5 analyze_noise clientpc MCU2 xp_config_scripted \
		p_progPLD p_progMCU can_utils.o getStatus p_eeprom2 enableChannel \
		xgetStatus xp_config xsetThreshold xenableChannel \
		xMCU2 xp_eeprom2 xp_config_reset getChecksum x_getChecksum \
		getFWIDs getTHUBFWIDs MCUchecksum THUBchecksum xp_config_pm *_l \
		checkCan 

backup:
	rm -f pcan.tgz; \
	tar -zcf pcan.tgz $(DIST_FILES)

