TARGET = xcb

include(../../qpluginbase.pri)
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES = \
        main.cpp \
        qxcbintegration.cpp \
        qxcbconnection.cpp \
        qxcbscreen.cpp \
        qxcbwindow.cpp \
        qxcbwindowsurface.cpp

HEADERS = \
        qxcbobject.h \
        qxcbintegration.h \
        qxcbconnection.h \
        qxcbscreen.h \
        qxcbwindow.h \
        qxcbwindowsurface.h

LIBS += -lxcb

include (../fontdatabases/genericunix/genericunix.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
