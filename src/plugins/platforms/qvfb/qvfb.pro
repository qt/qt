TARGET = qvfbintegration
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms


SOURCES = main.cpp qvfbintegration.cpp qvfbwindowsurface.cpp
HEADERS = qvfbintegration.h qvfbwindowsurface.h

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
