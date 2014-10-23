TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += main.cpp \
    cluster.cpp \
    fanal.cpp \
    documentation.cpp \
    jstring.cpp \
    commandhandler.cpp \
    macrocluster.cpp \
    hopfield.cpp

HEADERS += \
    cluster.h \
    fanal.h \
    documentation.h \
    jstring.h \
    commandhandler.h \
    macrocluster.h \
    macros.h \
    utils.h \
    hopfield.h

include(../Common.pri)

LIBS += -fopenmp -ltbb
