# $Id$
all:		pcanloop pc p_config

pcanloop:	pcanloop.cc
	$(CXX) $^ -o $@ -lpcan

pc:	pc.c
	$(CC) $^ -o $@

p_config:	p_config.cc
	$(CXX) $^ -o $@ -lpcan

