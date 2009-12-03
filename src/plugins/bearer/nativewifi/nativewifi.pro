TARGET = qnativewifibearer
include(../../qpluginbase.pri)

QT += network

DEFINES += BEARER_ENGINE

HEADERS += qnativewifiengine.h platformdefs.h
SOURCES += qnativewifiengine.cpp main.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/bearer
target.path += $$[QT_INSTALL_PLUGINS]/bearer
INSTALLS += target
