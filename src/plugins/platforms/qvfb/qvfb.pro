TARGET = qvfbintegration
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms


SOURCES = main.cpp qvfbintegration.cpp qwindowsurface_qvfb.cpp
HEADERS = qvfbintegration.h qwindowsurface_qvfb.h

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
