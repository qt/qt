DESTDIR = ../../bin
QT += declarative script network sql
# Input
HEADERS += qfxviewer.h 
SOURCES += main.cpp qfxviewer.cpp 

include($$QT_SOURCE_TREE/tools/shared/deviceskin/deviceskin.pri)

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG += console
