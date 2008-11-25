CFLAGS=-I../inc -fPIC

all: libpcan.a libpcan.so

libpcan.o : libpcan.c

libpcan.so : libpcan.c
	$(CC) $(CFLAGS) -shared -c -o $@ $<

libpcan.a : libpcan.o
	$(AR) -ru $@ $^
	ranlib $@

clean:
	$(RM) libpcan.a libpcan.so *.o

distclean:
	$(RM) libpcan.a libpcan.so *.o
