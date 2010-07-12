TARGET = eglintegration
TEMPLATE = lib
CONFIG += plugin

QT += opengl

SOURCES =   main.cpp \
            eglintegration.cpp \
            ../eglconvenience/qeglconvenience.cpp
HEADERS =   eglintegration.h \
            ../eglconvenience/qeglconvenience.h

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
