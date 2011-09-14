TARGET = qwayland
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

DEFINES += Q_PLATFORM_WAYLAND
DEFINES += $$QMAKE_DEFINES_WAYLAND

SOURCES =   main.cpp \
            qwaylandintegration.cpp \
            qwaylandnativeinterface.cpp \
            qwaylandshmsurface.cpp \
            qwaylandinputdevice.cpp \
            qwaylandcursor.cpp \
            qwaylanddisplay.cpp \
            qwaylandwindow.cpp \
            qwaylandscreen.cpp \
            qwaylandshmwindow.cpp \
            qwaylandclipboard.cpp

HEADERS =   qwaylandintegration.h \
            qwaylandnativeinterface.h \
            qwaylandcursor.h \
            qwaylanddisplay.h \
            qwaylandwindow.h \
            qwaylandscreen.h \
            qwaylandshmsurface.h \
            qwaylandbuffer.h \
            qwaylandshmwindow.h \
            qwaylandclipboard.h

INCLUDEPATH += $$QMAKE_INCDIR_WAYLAND
LIBS += $$QMAKE_LIBS_WAYLAND
QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_WAYLAND

!isEmpty(QMAKE_LFLAGS_RPATH):!contains(QT_CONFIG, no-pkg-config) {
    WAYLAND_NEEDS_RPATH = $$system(pkg-config --libs-only-L wayland-client)
    !isEmpty(WAYLAND_NEEDS_RPATH) {
        !isEmpty(QMAKE_LIBDIR_WAYLAND):QMAKE_LFLAGS += $${QMAKE_LFLAGS_RPATH}$${QMAKE_LIBDIR_WAYLAND}
    }
}

INCLUDEPATH += $$PWD

include ($$PWD/gl_integration/gl_integration.pri)
include ($$PWD/windowmanager_integration/windowmanager_integration.pri)

include (../fontdatabases/genericunix/genericunix.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target

