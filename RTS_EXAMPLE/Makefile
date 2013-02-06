STROOT_DIR = /data2/stardaq/StRoot
# for tofcontrol.starp.bnl.gov:
#STROOT_DIR = /home/Software/StRoot

STROOT_SRC_DIR = $(STROOT_DIR)/RTS/src

-include $(STROOT_SRC_DIR)/rtsmakefile.def	# for RTS
-include $(STROOT_SRC_DIR)/rtsplus.def

INCLUDES = -I. -I$(STROOT_SRC_DIR) -I$(STROOT_DIR)/RTS/include -I$(STROOT_DIR)/RTS/trg/include


LDLIBS = 
DEFINES += -DRTS_LOG_COLORED


VOBJS += libtof.a libtrg.a libmtd.a \
	libdaqReader.a libsfs.a librtslog.a
VPATH +=  $(STROOT_SRC_DIR)/DAQ_TOF \
	$(STROOT_SRC_DIR)/DAQ_TRG \
	$(STROOT_SRC_DIR)/DAQ_MTD \
	$(STROOT_SRC_DIR)/DAQ_READER  $(STROOT_SRC_DIR)/SFS $(STROOT_SRC_DIR)/LOG 


BINS = rts_example rts_errors

all:   $(BINS)

//$(VOBJS):

rts_test: rts_test.o $(VOBJS)
	$(LINK.o) -o $@ $(LDLIBS) -Wl,--whole-archive $^ -Wl,--no-whole-archive

rts_example: rts_example.o $(VOBJS)
	$(LINK.o) -o $@ $(LDLIBS) -Wl,--whole-archive $^ -Wl,--no-whole-archive

rts_errors: rts_errors.o $(VOBJS)
	$(LINK.o) -o $@ $(LDLIBS) -Wl,--whole-archive $^ -Wl,--no-whole-archive


