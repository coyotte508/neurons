TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
QT += qml quick
TARGET = neurons

SOURCES += main.cpp \
    cluster.cpp \
    fanal.cpp \
    documentation.cpp \
    jstring.cpp \
    commandhandler.cpp \
    macrocluster.cpp \
    hopfield.cpp \
    graphics/neuronsgrid.cpp \
    mnist.cpp \
    easycliquenetwork.cpp \
    graphics/mnistgraphics.cpp \
    exhaustive.cpp

HEADERS += \
    cluster.h \
    fanal.h \
    documentation.h \
    jstring.h \
    commandhandler.h \
    macrocluster.h \
    macros.h \
    utils.h \
    hopfield.h \
    graphics/neuronsgrid.h \
    mnist.h \
    easycliquenetwork.h \
    graphics/mnistgraphics.h \
    exhaustive.h

include(../Common.pri)

LIBS += -fopenmp -ltbb

OTHER_FILES += \
    ../../bin/qml/neurongrid.qml \
    ../../bin/qml/mnist.qml

#QMAKE_CXXFLAGS += -fpermissive
