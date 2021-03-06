######################################################################
# Automatically generated by qmake (2.01a) Sun Nov 9 14:04:03 2008
######################################################################
DEFINES += QT_NO_DEBUG_OUTPUT

TEMPLATE = lib
TARGET = anaconda
VERSION = 1.0.0
#DEPENDPATH += . ../fakepcan
INCLUDEPATH += . ../inc
QT += sql
CONFIG -= release
CONFIG += debug

extralib.target = extra
extralib.commands = cd ..; ./makeVersionFile.sh
extralib.depends =
 
QMAKE_EXTRA_TARGETS += extralib
#PRE_TARGETDEPS = extra

# STATIC = no
# contains(STATIC, yes) {
#   CONFIG += static
# }
# !contains(STATIC, yes) {
#   CONFIG += shared
# }
#message(PCAN: $$PCAN)
#contains(PCAN, fake) {
#  LIBS += -L../fakepcan -lpcan
#  DEFINES += FAKEPCAN
#} else {
#  LIBS += -lpcan
#}

message(CONFIG: $$CONFIG)
#debug {
#  LIBS += -L../fakepcan -lpcan
#}


CONFIG(epics) {
	message("epics defined")
#	DEFINES += WITH_EPICS
	DEFINES += WITH_EPICS UNIX

	EPICSDIR = /usr/local/epics
	EZCADIR = $${EPICSDIR}/extensions/srz/ezca

	INCLUDEPATH += $$EZCADIR $${EPICSDIR}/base/include $${EPICSDIR}/base/include/os/Linux 
	INCLUDEPATH += $${EPICSDIR}/extensions/include
}

CONFIG(mtd) {
	message("mtd defined")
	DEFINES += MTD
}

CONFIG(vcan) {
	message("vcan defined")
	DEFINES += VCAN
}

DESTDIR = ../lib

# Input
HEADERS += AnExceptions.h \
           AnAddress.h \
           AnCanObject.h AnCanNet.h AnBoard.h \
           AnTcpu.h AnTdig.h AnTdc.h AnTdcConfig.h \
           AnThub.h AnSerdes.h \
           AnRoot.h AnAgent.h AnMaster.h \
           AnEpicsLogger.h \
           AnRdMsg.h AnLog.h version.h

SOURCES += AnAddress.cpp \
           AnCanObject.cpp AnCanNet.cpp AnBoard.cpp \
           AnTcpu.cpp AnTdig.cpp AnTdc.cpp AnTdcConfig.cpp \
           AnThub.cpp AnSerdes.cpp \
           AnRoot.cpp AnAgent.cpp AnMaster.cpp \
           AnEpicsLogger.cpp \
           AnRdMsg.cpp AnLog.cpp \
           can_utils.cpp

