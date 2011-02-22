STROOT_SRC_DIR = /data2/stardaq/StRoot/RTS/src
STROOT_DIR = /data2/stardaq
-include $(STROOT_SRC_DIR)/rtsmakefile.def	# for RTS
-include $(STROOT_SRC_DIR)/rtsplus.def

INCLUDES = -I. -I$(STROOT_SRC_DIR) -I$(STROOT_DIR)/StRoot/RTS/include -I$(STROOT_DIR)/StRoot/RTS/trg/include



LDLIBS = 
DEFINES += -DRTS_LOG_COLORED


VOBJS += libbsmd.a libbtow.a libemc.a libesmd.a libfpd.a libftp.a libl3.a libpmd.a libpp2pp.a \
	libric.a libsc.a libssd.a libsvt.a libtof.a libtpc.a libtpcfcf.a libtpx.a libtrg.a \
	libfgt.a libmtd.a libhlt.a libetow.a \
	libdaqReader.a libsfs.a librtslog.a
VPATH +=  $(STROOT_SRC_DIR)/DAQ_BSMD $(STROOT_SRC_DIR)/DAQ_BTOW $(STROOT_SRC_DIR)/DAQ_EMC \
	$(STROOT_SRC_DIR)/DAQ_ESMD $(STROOT_SRC_DIR)/DAQ_FPD $(STROOT_SRC_DIR)/DAQ_FTP \
	$(STROOT_SRC_DIR)/DAQ_L3 $(STROOT_SRC_DIR)/DAQ_PMD \
	$(STROOT_SRC_DIR)/DAQ_PP2PP $(STROOT_SRC_DIR)/DAQ_RIC $(STROOT_SRC_DIR)/DAQ_SC \
	$(STROOT_SRC_DIR)/DAQ_SSD $(STROOT_SRC_DIR)/DAQ_SVT $(STROOT_SRC_DIR)/DAQ_TOF \
	$(STROOT_SRC_DIR)/DAQ_TPC $(STROOT_SRC_DIR)/FCF $(STROOT_SRC_DIR)/DAQ_HLT \
	$(STROOT_SRC_DIR)/DAQ_ETOW \
	$(STROOT_SRC_DIR)/DAQ_TPX $(STROOT_SRC_DIR)/DAQ_TRG $(STROOT_SRC_DIR)/DAQ_FGT \
	$(STROOT_SRC_DIR)/DAQ_MTD \
	$(STROOT_SRC_DIR)/DAQ_READER  $(STROOT_SRC_DIR)/SFS $(STROOT_SRC_DIR)/LOG 


BINS = rts_example rts_errors rts_test

all:   $(BINS)

//$(VOBJS):

rts_test: rts_test.o $(VOBJS)
	$(LINK.o) -o $@ $(LDLIBS) -Wl,--whole-archive $^ -Wl,--no-whole-archive

rts_example: rts_example.o $(VOBJS)
	$(LINK.o) -o $@ $(LDLIBS) -Wl,--whole-archive $^ -Wl,--no-whole-archive

rts_errors: rts_errors.o $(VOBJS)
	$(LINK.o) -o $@ $(LDLIBS) -Wl,--whole-archive $^ -Wl,--no-whole-archive


