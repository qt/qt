TARGET = distfieldgen
TEMPLATE = app

QT += declarative opengl

CONFIG   += console
CONFIG   -= app_bundle
DESTDIR = ../../bin

INCLUDEPATH += $$PWD/../../src/3rdparty/harfbuzz/src

SOURCES += main.cpp
