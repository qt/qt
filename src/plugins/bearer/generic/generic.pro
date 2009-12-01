TARGET = qgenericbearer
include(../../qpluginbase.pri)

QT += network

DEFINES += BEARER_ENGINE

HEADERS += qgenericengine.h
SOURCES += qgenericengine.cpp main.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/bearer
target.path += $$[QT_INSTALL_PLUGINS]/bearer
INSTALLS += target
