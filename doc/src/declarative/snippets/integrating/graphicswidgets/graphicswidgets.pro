TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

HEADERS += redsquare.h \
           bluecircle.h

SOURCES += shapesplugin.cpp

DESTDIR = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp

