######################################################################
# Automatically generated by qmake (2.01a) Sun Nov 9 14:04:03 2008
######################################################################

TEMPLATE = lib
TARGET = anaconda
VERSION = 1.0.0
DEPENDPATH += . ../fakepcan
INCLUDEPATH += . ../inc
QT += sql

DESTDIR = ../lib

macx {
 LIBS += -L../fakepcan -lpcan
}
# Input
HEADERS += AnLAddress.h AnHAddress.h AnAddress.h \
           AnCanObject.h AnCanNet.h AnBoard.h \
           AnTcpu.h AnTdig.h AnTdc.h \
           AnThub.h AnSerdes.h \
           AnRoot.h AnSock.h

SOURCES += AnLAddress.cpp AnHAddress.cpp AnAddress.cpp \
           AnCanObject.cpp AnCanNet.cpp AnBoard.cpp \
           AnTcpu.cpp AnTdig.cpp AnTdc.cpp \
           AnThub.cpp AnSerdes.cpp \
           AnRoot.cpp AnSock.cpp
