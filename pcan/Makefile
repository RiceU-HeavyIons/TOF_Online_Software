# $Id$
all:		pcanloop pc p_config change_mcu_program

pcanloop:	pcanloop.cc
	$(CXX) $^ -o $@ -lpcan

pc:	pc.c
	$(CC) $^ -o $@

p_config:	p_config.cc
	$(CXX) $^ -o $@ -lpcan

change_mcu_program:	change_mcu_program.cc
	$(CXX) -Wall $^ -o $@ -lpcan

