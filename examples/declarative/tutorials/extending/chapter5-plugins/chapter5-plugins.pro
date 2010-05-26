TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp

HEADERS += musician.h \
           instrument.h \
           musicplugin.h

SOURCES += musician.cpp \
           instrument.cpp \
           musicplugin.cpp

symbian {
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1
}
