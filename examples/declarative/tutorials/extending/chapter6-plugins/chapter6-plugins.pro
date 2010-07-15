TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp

HEADERS += piechart.h \
           pieslice.h \
           chartsplugin.h

SOURCES += piechart.cpp \
           pieslice.cpp \
           chartsplugin.cpp

symbian {
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1
}
