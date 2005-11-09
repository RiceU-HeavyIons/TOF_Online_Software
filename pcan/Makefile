# $Id$
all:		pcanloop pc p_config change_mcu_program pcan_find setThreshold \
		p_config_run5 p_config_bench analyze_noise

pcanloop:	pcanloop.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pc:	pc.c
	$(CC) -Wall $^ -o $@

p_config:	p_config.cc
	$(CXX) -Wall $^ -o $@ -lpcan

change_mcu_program:	change_mcu_program.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pcan_find:	pcan_find.cc
	$(CXX) -Wall $^ -o $@ -lpcan

