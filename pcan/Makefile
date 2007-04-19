# $Id$
all:		pcanloop pc p_config change_mcu_program pcan_find setThreshold \
		p_config_run5 p_config_bench analyze_noise clientpc \
		p_progPLD pcanloop_TDIGD p_progMCU

pcanloop:	pcanloop.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pcanloop_TDIGD:	pcanloop_TDIGD.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pc:	pc.c
	$(CC) -Wall $^ -o $@

p_config:	p_config.cc
	$(CXX) -Wall $^ -o $@ -lpcan

change_mcu_program:	change_mcu_program.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pcan_find:	pcan_find.cc
	$(CXX) -Wall $^ -o $@ -lpcan

pcanloop2:	pcanloop2.cc
	$(CXX) -Wall $^ -o $@ -lpcan

clientpc:	clientpc.c
	$(CXX) -Wall $^ -o $@ -lpcan

p_progPLD:	p_progPLD.cc
	$(CXX) -Wall $^ -o $@ -lpcan

p_progMCU:	p_progMCU.cc
	$(CXX) -Wall $^ -o $@ -lpcan

clean:
	rm -f *.o pcanloop pc p_config change_mcu_program pcan_find setThreshold \
		p_config_run5 p_config_bench analyze_noise clientpc \
		p_progPLD pcanloop_TDIGD
