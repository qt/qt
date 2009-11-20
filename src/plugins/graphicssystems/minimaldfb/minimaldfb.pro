TARGET = qminimaldfb
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

QMAKE_CXXFLAGS += -I/usr/local/include/directfb
LIBS += -L/usr/local/lib -ldirectfb -lfusion -ldirect -lpthread

SOURCES = main.cpp qgraphicssystem_minimaldfb.cpp qwindowsurface_minimaldfb.cpp qblitter_directfb.cpp
HEADERS = qgraphicssystem_minimaldfb.h qwindowsurface_minimaldfb.h qblitter_directfb.h

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
