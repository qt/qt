TARGET = qvncgraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

SOURCES = main.cpp qgraphicssystem_vnc.cpp
HEADERS = qgraphicssystem_vnc.h

HEADERS += qvncserver.h
SOURCES += qvncserver.cpp

HEADERS += qvnccursor.h
SOURCES += qvnccursor.cpp

include(../fb_base/fb_base.pri)

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems

INSTALLS += target
