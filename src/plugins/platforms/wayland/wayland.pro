TARGET = qwayland
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES =   main.cpp \
            qwaylandintegration.cpp \
            qwaylandshmsurface.cpp \
            qwaylanddrmsurface.cpp \
            qwaylandinputdevice.cpp \
            qwaylandglcontext.cpp \
    qwaylandcursor.cpp \
    qwaylanddisplay.cpp \
    qwaylandwindow.cpp \
    qwaylandscreen.cpp

HEADERS =   qwaylandintegration.h \
    qwaylandcursor.h \
    qwaylanddisplay.h \
    qwaylandwindow.h \
    qwaylandscreen.h \
    qwaylandglcontext.h \
    qwaylandshmsurface.h \
    qwaylanddrmsurface.h \
    qwaylandbuffer.h

contains(QT_CONFIG, opengl) {
    QT += opengl
}
LIBS += -lwayland-client -lxkbcommon -lEGL
unix {
	CONFIG += link_pkgconfig
	PKGCONFIG += libdrm
}

include (../fontdatabases/fontconfig/fontconfig.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
