TARGET = qminimalgraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

SOURCES = main.cpp qgraphicssystem_minimal.cpp qwindowsurface_minimal.cpp
HEADERS = qgraphicssystem_minimal.h qwindowsurface_minimal.h

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
