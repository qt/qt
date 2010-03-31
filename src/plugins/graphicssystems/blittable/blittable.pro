TARGET = qblittablegraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

SOURCES = main.cpp
HEADERS =  qblittable_image.h  \
           qgraphicssystem_blittable.h

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
