
CXX = g++
CC = gcc
LD = g++ 

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --new --libs)
ROOTGLIBS    := $(shell root-config --glibs)

CFLAGS = -O3  -I. -ITPC
CXXFLAGS = ${CFLAGS} $(ROOTCFLAGS)

#Uncomment for Solaris
#LDLIBS = -lsocket -lnsl

READER_FILES = msgNQLib.o evpSupport.o evpReader.o tpcReader.o svtReader.o ftpReader.o \
                emcReader.o tofReader.o pmdReader.o fpdReader.o ricReader.o l3Reader.o \
                trgReader.o trgReader12.o trgReader20.o trgReader21.o trgReader22.o ssdReader.o pp2ppReader.o \
		sanityCheckers.o scReader.o fcfAfterburner.o fcfClass.o tpcFCFReader.o

AUX_FILES = rtsLogUnix.o

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --new --libs)
ROOTGLIBS    := $(shell root-config --glibs)

special: special.o libevp.a
	${CXX} special.o libevp.a ${LDLIBS} -o special $(ROOTLIBS)

libevp.a : ${READER_FILES} ${AUX_FILES}
	ar -rucv libevp.a $?

clean:
	rm -f *.o *.a *.so *.tar special

tar:
	tar cf evp_reader.tar *.c *.C *.h *.hh SECTOR TRG L3 TPC SVT Makefile
