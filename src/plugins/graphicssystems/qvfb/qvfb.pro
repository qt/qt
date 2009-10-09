TARGET = qvfbgraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems



SOURCES = main.cpp qgraphicssystem_qvfb.cpp qwindowsurface_qvfb.cpp
HEADERS = qgraphicssystem_qvfb.h qwindowsurface_qvfb.h

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
