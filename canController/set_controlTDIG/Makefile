# $Id$

#INSTALL = /usr/bin/install
INSTALL = /bin/mv -f
BINDIR = Release
EXE_FILE = set_control

all:	$(EXE_FILE)

$(EXE_FILE):	$(EXE_FILE).cc
	$(CXX) -Wall $^ -o $@ -lpcan

install:: $(EXE_FILE)
	@if [ -d $(BINDIR) ]; then set +x; \
	else (set -x; mkdir $(BINDIR)); fi
	$(INSTALL)  $^  $(BINDIR)/$^
