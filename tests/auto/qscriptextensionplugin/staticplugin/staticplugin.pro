TEMPLATE = lib
CONFIG += static plugin
CONFIG -= create_prl    # not needed, and complicates debug/release
SOURCES = staticplugin.cpp
RESOURCES = staticplugin.qrc
QT = core script
TARGET = staticplugin
DESTDIR = ../plugins/script
