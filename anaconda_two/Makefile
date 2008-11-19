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

OBJS = lib/tofc.o lib/tdc_config.o lib/tdc_status.o

LIBS = $(GTK_LIBS) $(PCAN_LIBS)  $(ROOT_LIBS)

DIRS = lib_dir test_dir

TARGET=tofcon

all: $(DIRS) $(TARGET)

lib_dir: 
	cd oldlib; $(MAKE)

test_dir: 
	cd test; $(MAKE)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) main.cpp $(OBJS) $(LIBS)


clean:
	cd lib; $(MAKE) clean
	cd test; $(MAKE) test
	rm -f $(OBJS) $(TARGET)
	
rsync:
	rsync -a koheik@lone:/homes/koheik/workspace/anaconda_two/ .

