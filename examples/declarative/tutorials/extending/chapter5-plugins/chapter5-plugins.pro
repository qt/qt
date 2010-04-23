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
