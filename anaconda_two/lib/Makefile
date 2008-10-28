#PCAN=../tray_cosmic/rcan/peak-linux-driver-3.25
PCAN=/usr
PCAN_CFLAGS=-I$(PCAN)/lib -I$(PCAN)/driver
PCAN_LIBS=-L$(PCAN)/lib -lpcan
 GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`
 GTK_LIBS=`pkg-config --libs gtk+-2.0`
# ROOT_CFLAGS=`root-config --cflags`
# ROOT_LIBS=`root-config --glibs`

CXXFLAGS = -O2 -g -Wall -fmessage-length=0 \
 $(GTK_CFLAGS) $(PCAN_CFLAGS) $(ROOT_CFLAGS)

OBJS = tofc.o

LIBS = $(GTK_LIBS) $(PCAN_LIBS)  $(ROOT_LIBS)

tofc.o: tofc.cpp

clean:
	rm -f $(OBJS)
