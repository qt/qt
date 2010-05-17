TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

HEADERS += musician.h \
           instrument.h \
           musicplugin.h

SOURCES += musician.cpp \
           instrument.cpp \
           musicplugin.cpp

DESTDIR = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp

symbian {
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1
}
