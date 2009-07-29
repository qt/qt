TEMPLATE        = app
CONFIG += qt uic
DESTDIR = ../../bin
QT += declarative script network sql
# Input
HEADERS += qmlviewer.h
SOURCES += main.cpp qmlviewer.cpp

FORMS = recopts.ui

include($$QT_SOURCE_TREE/tools/shared/deviceskin/deviceskin.pri)

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

