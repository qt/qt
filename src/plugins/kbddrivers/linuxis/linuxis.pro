TARGET = linuxiskbdhandler
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/kbddrivers
target.path = $$[QT_INSTALL_PLUGINS]/kbddrivers
INSTALLS += target

CONFIG+=no_tr

HEADERS = linuxiskbddriverplugin.h linuxiskbdhandler.h
SOURCES = linuxiskbddriverplugin.cpp linuxiskbdhandler.cpp
