TARGET = linuxismousehandler
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/mousedrivers
target.path = $$[QT_INSTALL_PLUGINS]/mousedrivers
INSTALLS += target

HEADERS = linuxismousedriverplugin.h linuxismousehandler.h
SOURCES = linuxismousedriverplugin.cpp linuxismousehandler.cpp

