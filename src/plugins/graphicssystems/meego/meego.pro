TARGET = qmeegographicssystem
include(../../qpluginbase.pri)

QT += gui opengl

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

HEADERS = qmeegographicssystem.h qmeegopixmapdata.h qmeegoextensions.h
SOURCES = qmeegographicssystem.cpp qmeegographicssystem.h qmeegographicssystemplugin.h qmeegographicssystemplugin.cpp qmeegopixmapdata.h qmeegopixmapdata.cpp qmeegoextensions.h qmeegoextensions.cpp

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target

