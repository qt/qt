TARGET = qwayland
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

DEFINES += Q_PLATFORM_WAYLAND
DEFINES += $$QMAKE_DEFINES_WAYLAND

SOURCES =   main.cpp \
            qwaylandintegration.cpp \
            qwaylandshmsurface.cpp \
            qwaylandinputdevice.cpp \
            qwaylandcursor.cpp \
            qwaylanddisplay.cpp \
            qwaylandwindow.cpp \
            qwaylandscreen.cpp \
            qwaylandshmwindow.cpp

HEADERS =   qwaylandintegration.h \
            qwaylandcursor.h \
            qwaylanddisplay.h \
            qwaylandwindow.h \
            qwaylandscreen.h \
            qwaylandshmsurface.h \
            qwaylanddrmsurface.h \
            qwaylandbuffer.h \
            qwaylandinclude.h \
            qwaylandeglwindow.h \
            qwaylandshmwindow.h

INCLUDEPATH += $$QMAKE_INCDIR_WAYLAND
LIBS += $$QMAKE_LIBS_WAYLAND
QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_WAYLAND

contains(QT_CONFIG, opengles2) {
    QT += opengl
    LIBS += -lwayland-egl -lEGL

    SOURCES += qwaylanddrmsurface.cpp \
            qwaylandglcontext.cpp \
            ../eglconvenience/qeglconvenience.cpp \
            qwaylandeglwindow.cpp

    HEADERS += qwaylandglcontext.h \
            ../eglconvenience/qeglconvenience.h \

    DEFINES += QT_WAYLAND_GL_SUPPORT
}

include (../fontdatabases/genericunix/genericunix.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
