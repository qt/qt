TEMPLATE = lib
CONFIG += plugin
QT += opengl

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/gfxdrivers

TARGET = hybridscreen
target.path = $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS += target

HEADERS	= hybridscreen.h \
          hybridsurface.h
SOURCES	= hybridscreen.cpp \
          hybridsurface.cpp \
          hybridplugin.cpp

