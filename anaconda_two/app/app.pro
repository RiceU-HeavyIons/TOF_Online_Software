######################################################################
# Automatically generated by qmake (2.01a) Thu Nov 20 20:14:57 2008
######################################################################

TEMPLATE = app
TARGET = Anaconda2
DEPENDPATH += .
INCLUDEPATH += . ../inc ../src
LIBS += -L../lib -lanaconda
CONFIG += sql

# Input
HEADERS += CanObject.h \
           Config.h \
           DetailView.h \
           KLevel2View.h \
           KLevel3View.h \
           KMainWindow.h \
           KTcpuView.h \
           KTdigView.h \
           TableViewModel.h \
           TdigTableModel.h
SOURCES += CanObject.cpp \
           Config.cpp \
           DetailView.cpp \
           KLevel2View.cpp \
           KLevel3View.cpp \
           KMainWindow.cpp \
           KTcpuView.cpp \
           KTdigView.cpp \
           main.cpp \
           TableViewModel.cpp \
           TdigTableModel.cpp
RESOURCES += toolbar.qrc