TARGET = qwayland
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES =   main.cpp \
            qwaylandintegration.cpp \
            qwaylandshmsurface.cpp \
            qwaylanddrmsurface.cpp \
            qwaylandinputdevice.cpp \
            qwaylandglcontext.cpp

HEADERS =   qwaylandintegration.h \
            qwaylandwindowsurface.h

contains(QT_CONFIG, opengl) {
    QT += opengl
}
LIBS += -lwayland-client -lxkbcommon -lEGL -lGLESv2
unix {
	CONFIG += link_pkgconfig
	PKGCONFIG += libdrm
}

include (../fontdatabases/fontconfig/fontconfig.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
