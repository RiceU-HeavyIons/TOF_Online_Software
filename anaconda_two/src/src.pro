######################################################################
# Automatically generated by qmake (2.01a) Sun Nov 9 14:04:03 2008
######################################################################

TEMPLATE = lib
TARGET = anaconda
VERSION = 1.0.0
DEPENDPATH += . 
INCLUDEPATH += . ../inc
QT += sql

DESTDIR = ../lib
mac {
 LIBS += -L../fakepcan -lpcan
}
# Input
HEADERS += AnLAddress.h AnHAddress.h\
	AnCanObject.h AnBoard.h \
	AnTcpu.h AnTdig.h AnTdc.h \
	AnThub.h \
	AnRoot.h AnSock.h

SOURCES += AnLAddress.cpp AnHAddress.cpp\
	AnCanObject.cpp AnBoard.cpp \
	AnTcpu.cpp AnTdig.cpp AnTdc.cpp \
	AnThub.cpp \
	AnRoot.cpp AnSock.cpp
