TARGET = qdirectfbgraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

QMAKE_CXXFLAGS += -I/usr/include/directfb
LIBS += -ldirectfb -lfusion -ldirect -lpthread

SOURCES = main.cpp qgraphicssystem_minimaldfb.cpp qwindowsurface_minimaldfb.cpp
HEADERS = qgraphicssystem_minimaldfb.h qwindowsurface_minimaldfb.h

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
