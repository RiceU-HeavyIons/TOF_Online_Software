# $Id$

CXXFLAGS += -Wall

all:		pcanloop pc p_config change_mcu_program pcan_find setThreshold \
		p_config_run5 p_config_bench analyze_noise clientpc \
		p_progPLD p_progMCU getStatus p_eeprom2 enableChannel

pcanloop:	pcanloop.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pc:	pc.c
	$(CC) -Wall $^ -o $@

p_config:	p_config.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

p_eeprom2:	p_eeprom2.o can_utils.o
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

p_progPLD:	p_progPLD.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

p_progMCU:	p_progMCU.o can_utils.o
	$(CXX)  $^ -o $@ -lpcan

clean:
	rm -f *.o pcanloop pc p_config change_mcu_program pcan_find setThreshold \
		p_config_run5 p_config_bench analyze_noise clientpc \
		p_progPLD p_progMCU can_utils.o getStatus p_eeprom2
