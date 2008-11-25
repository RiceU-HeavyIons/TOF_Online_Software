all: makefile.qmake qmake

makefile.qmake: src.pro
	qmake -o $@ $<

qmake: makefile.qmake
	$(MAKE) -f $<

clean: makefile.qmake
	$(MAKE) -f $< clean
	$(RM) -f $<
rsync:
	rsync -a koheik@lone:/homes/koheik/workspace/anaconda_two/src/ .