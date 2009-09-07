TARGET = qvncgraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems



SOURCES = main.cpp qgraphicssystem_vnc.cpp qwindowsurface_vnc.cpp
HEADERS = qgraphicssystem_vnc.h qwindowsurface_vnc.h

HEADERS += qvncserver.h
SOURCES += qvncserver.cpp


target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
