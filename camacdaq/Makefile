# Makefile for the ROOT test programs.
# This Makefile shows nicely how to compile and link applications
# using the ROOT libraries on all supported platforms.
#
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

ARCH          = linux

CXX           =
ObjSuf        = o
SrcSuf        = cxx
ExeSuf        =
DllSuf        = so
OutPutOpt     = -o 

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --new --libs)
ROOTGLIBS    := $(shell root-config --glibs)

ifeq ($(ARCH),win32)
PLATFORM      = win32
endif
ifeq ($(ARCH),win32gdk)
PLATFORM      = win32
endif

ifeq ($(ARCH),hurddeb)
# GNU/Hurd
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),aix)
# IBM AIX xlC 4.x
CXX           = xlC
CXXFLAGS      = -O
LD            = xlC
LDFLAGS       = -O
SOFLAGS       =
ROOTLIBS     := $(shell root-config --nonew --libs)
ROOTGLIBS    := $(shell root-config --nonew --glibs)
endif

ifeq ($(ARCH),aix5)
# IBM AIX xlC 5.x
CXX           = xlC
CXXFLAGS      = -O
LD            = xlC
LDFLAGS       = -O
SOFLAGS       =
ROOTLIBS     := $(shell root-config --nonew --libs)
ROOTGLIBS    := $(shell root-config --nonew --glibs)
endif

ifeq ($(ARCH),aixegcs)
# IBM AIX with GCC
CXX           = g++
CXXFLAGS      = -O
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),solaris)
# Solaris CC
CXX           = /opt/SUNWspro/bin/CC
CXXFLAGS      = -O -KPIC
LD            = /opt/SUNWspro/bin/CC
LDFLAGS       = -O
SOFLAGS       = -G
endif

ifeq ($(ARCH),solarisCC5)
# Solaris CC 5.0
CXX           = CC
CXXFLAGS      = -O -KPIC
LD            = CC
LDFLAGS       = -O
SOFLAGS       = -G
endif

ifeq ($(ARCH),solarisegcs)
# Solaris egcs
CXX           = g++
CXXFLAGS      = -O -fPIC
LD            = CC
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),solarisgcc)
# Solaris gcc
CXX           = g++
CXXFLAGS      = -O -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),solariskcc)
# Solaris kcc
CXX           = KCC
CXXFLAGS      = -O4 -KPIC
LD            = KCC
LDFLAGS       = -O4
SOFLAGS       =
endif

ifeq ($(ARCH),solarisx86)
# Solaris CC on Intel
CXX           = CC
CXXFLAGS      = -O -KPIC
LD            = CC
LDFLAGS       = -O
SOFLAGS       = -G
endif

ifeq ($(ARCH),alphagcc)
# Alpha/OSF with g++
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -Wl,-expect_unresolved,* -shared
endif

ifeq ($(ARCH),alphaegcs)
# Alpha/OSF with egcs
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -Wl,-expect_unresolved,* -shared
endif

ifeq ($(ARCH),alphakcc)
# Alpha/OSF with kai compiler (not yet valid)
CXX           = g++
CXXFLAGS      = -O -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -Wl,-expect_unresolved,* -shared
endif

ifeq ($(ARCH),alphacxx6)
# Alpha/OSF with cxx6
CXX           = cxx
CXXFLAGS      = -O0
LD            = cxx
LDFLAGS       = -O
SOFLAGS       = -Wl,-expect_unresolved,* -shared
endif

ifeq ($(ARCH),alphacxx)
# Alpha/OSF with cxx5
CXX           = cxx
CXXFLAGS      = -O
LD            = cxx
LDFLAGS       = -O
SOFLAGS       = -Wl,-expect_unresolved,* -call_shared
endif

ifeq ($(ARCH),linuxrh51)
# Linux with gcc 2.7.2.x
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxrh42)
# Linux with gcc 2.7.2.x (RedHat 4.2)
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxdeb)
# Linux with gcc 2.7.2.x
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxdeb2)
# Linux with gcc 2.7.2.x
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxsuse6)
# Linux with gcc 2.7.2.x
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linux)
# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxkcc)
# Linux with the KAI compiler
CXX           = KCC
CXXFLAGS      = -fPIC +K0
LD            = KCC
LDFLAGS       = -O
SOFLAGS       =
endif

ifeq ($(ARCH),linuxicc)
# Linux with Intel icc compiler
CXX           = icc
CXXFLAGS      = -O -KPIC
LD            = icc
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxppcegcs)
# MkLinux with egcs/glibc
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxia64gcc)
# Itanium Linux with gcc 2.9x
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxia64sgi)
# Itanium Linux with sgiCC
CXX           = sgiCC
CXXFLAGS      = -O -Wall -fPIC
LD            = gsgiCC
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxia64ecc)
# Itanium Linux with Intel ecc
CXX           = ecc
CXXFLAGS      = -O -KPIC
LD            = ecc
LDFLAGS       = -O -i_dynamic
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxalphaegcs)
# Alpha Linux with egcs
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxarm)
# ARM Linux with egcs
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),mklinux)
# MkLinux with libc5
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared
endif

ifeq ($(ARCH),freebsd)
# FreeBSD with libc5
CXX           = g++
CXXFLAGS      = -O -pipe -W -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared -Wl,-x
endif

ifeq ($(ARCH),freebsd4)
# FreeBSD with glibc
CXX           = g++
CXXFLAGS      = -O -pipe -W -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared -Wl,-x
endif

ifeq ($(PLATFORM),win32)
# Windows with the VC++ compiler
ObjSuf        = obj
SrcSuf        = cxx
ExeSuf        = .exe
DllSuf        = dll
OutPutOpt     = -out:
CXX           = cl
CXXOPT        = -O2
#CXXOPT        = -Z7
CXXFLAGS      = $(CXXOPT) -G5 -GR -MD -DWIN32 -D_WINDOWS -nologo \
                -DVISUAL_CPLUSPLUS -D_X86_=1 -D_DLL
LD            = link
LDOPT         = -opt:ref
#LDOPT         = -debug
LDFLAGS       = $(LDOPT) -pdb:none -nologo -nodefaultlib -incremental:no
SOFLAGS       = -DLL
SYSLIBS       = msvcrt.lib oldnames.lib kernel32.lib  ws2_32.lib mswsock.lib \
                advapi32.lib  user32.lib gdi32.lib comdlg32.lib winspool.lib \
                msvcirt.lib

ROOTLIBS     := $(shell root-config --nonew --libs)
ROOTGLIBS    := $(shell root-config --nonew --glibs)
endif

ifeq ($(CXX),)
$(error $(ARCH) invalid architecture)
endif

CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

#------------------------------------------------------------------------------
CAMAC_LIB_DIR 	= /home/jschamba/ks2915/lib
CAMAC_INC_DIR 	= /home/jschamba/ks2915/include
USERLIBS 	= -L$(CAMAC_LIB_DIR) -lks2915

CXXFLAGS     += -I$(CAMAC_INC_DIR)

HSIMPLEO      = hsimple.$(ObjSuf)
HSIMPLES      = hsimple.$(SrcSuf)
HSIMPLE       = hsimple$(ExeSuf)

DAQLOOPO      = daqloop.$(ObjSuf)
DAQLOOPS      = daqloop.$(SrcSuf)
DAQLOOP       = daqloop$(ExeSuf)

DAQLOOP2O      = daqloop2.$(ObjSuf)
DAQLOOP2S      = daqloop2.$(SrcSuf)
DAQLOOP2       = daqloop2$(ExeSuf)

DAQLOOP3O      = daqloop3.$(ObjSuf)
DAQLOOP3S      = daqloop3.$(SrcSuf)
DAQLOOP3       = daqloop3$(ExeSuf)

DAQLOOP4O      = daqloop4.$(ObjSuf)
DAQLOOP4S      = daqloop4.$(SrcSuf)
DAQLOOP4       = daqloop4$(ExeSuf)

DAQLOOP5O      = daqloop5.$(ObjSuf)
DAQLOOP5S      = daqloop5.$(SrcSuf)
DAQLOOP5       = daqloop5$(ExeSuf)

DAQCTL	      = daqctl$(Execsuf)

OBJS          = $(HSIMPLEO) $(DAQLOOPO) $(DAQLOOP2O) $(DAQLOOP3O) $(DAQLOOP4O) $(DAQLOOP5O)

PROGRAMS      = $(HSIMPLE) $(DAQLOOP) $(DAQCTL) $(DAQLOOP2) $(DAQLOOP3) $(DAQLOOP4) $(DAQLOOP5)

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:            $(PROGRAMS)

$(HSIMPLE):     $(HSIMPLEO)
		$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(DAQLOOP):     $(DAQLOOPO)
		$(LD) $(LDFLAGS) $^ $(LIBS) $(USERLIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(DAQLOOP2):    $(DAQLOOP2O)
		$(LD) $(LDFLAGS) $^ $(LIBS) $(USERLIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(DAQLOOP3):    $(DAQLOOP3O)
		$(LD) $(LDFLAGS) $^ $(LIBS) $(USERLIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(DAQLOOP4):    $(DAQLOOP4O)
		$(LD) $(LDFLAGS) $^ $(LIBS) $(USERLIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(DAQLOOP5):    $(DAQLOOP5O)
		$(LD) $(LDFLAGS) $^ $(LIBS) $(USERLIBS) $(OutPutOpt)$@
		@echo "$@ done"

clean:
		@rm -f $(OBJS) core

distclean:      clean
		@rm -f $(PROGRAMS) *Dict.* *.def *.exp \
		   *.root *.ps *.so .def so_locations
		@rm -rf cxx_repository

.SUFFIXES: .$(SrcSuf)

###

.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) -c $<
