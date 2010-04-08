TARGET = qtestlitegraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES = main.cpp qplatformintegration_testlite.cpp qwindowsurface_testlite.cpp
HEADERS = qplatformintegration_testlite.h qwindowsurface_testlite.h


HEADERS += x11util.h
SOURCES += x11util.cpp

LIBS += -lX11 -lXext

contains(QT_CONFIG, opengl) {
    QT += opengl
    HEADERS += qglxglcontext.h
    SOURCES += qglxglcontext.cpp
}

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
