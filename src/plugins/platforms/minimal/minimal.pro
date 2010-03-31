TARGET = qminimalgraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES =   main.cpp \
            qplatformintegration_minimal.cpp \
            qwindowsurface_minimal.cpp
HEADERS =   qplatformintegration_minimal.h \
            qwindowsurface_minimal.h

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
