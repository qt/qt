TARGET = qtestlitegraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

SOURCES = main.cpp qgraphicssystem_testlite.cpp qwindowsurface_testlite.cpp
HEADERS = qgraphicssystem_testlite.h qwindowsurface_testlite.h


HEADERS += x11util.h
SOURCES += x11util.cpp

LIBS += -lX11


target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
