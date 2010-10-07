TARGET = qwayland
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES =   main.cpp \
            qwaylandintegration.cpp \
            qwaylandwindowsurface.cpp

HEADERS =   qwaylandintegration.h \
            qwaylandwindowsurface.h

INCLUDEPATH += /usr/include/libdrm
LIBS += -lwayland-client -ldrm

include (../fontdatabases/fontconfig/fontconfig.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
