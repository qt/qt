TARGET = qtestlitegraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES = main.cpp qtestliteintegration.cpp qtestlitewindowsurface.cpp qtestlitewindow.cpp
HEADERS = qtestliteintegration.h qtestlitewindowsurface.h qtestlitewindow.h

LIBS += -lX11 -lXext

contains(QT_CONFIG, opengl) {
    QT += opengl
    HEADERS += qglxglcontext.h
    SOURCES += qglxglcontext.cpp
}

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
