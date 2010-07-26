TARGET = qeglfs
TEMPLATE = lib
CONFIG += plugin

QT += opengl

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES =   main.cpp \
            qeglfsintegration.cpp \
            ../eglconvenience/qeglconvenience.cpp
HEADERS =   qeglfsintegration.h \
            ../eglconvenience/qeglconvenience.h

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
