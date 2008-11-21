CFLAGS=-I../inc

all: libpcan.a

libpcan.o : libpcan.c

libpcan.a : libpcan.o
	$(AR) -ru $@ $^
	ranlib $@

clean:
	$(RM) libpcan.a *.o
