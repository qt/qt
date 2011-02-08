TARGET = xcb

include(../../qpluginbase.pri)
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES = \
        qxcbconnection.cpp \
        qxcbintegration.cpp \
        qxcbkeyboard.cpp \
        qxcbscreen.cpp \
        qxcbwindow.cpp \
        qxcbwindowsurface.cpp \
        main.cpp

HEADERS = \
        qxcbconnection.h \
        qxcbintegration.h \
        qxcbkeyboard.h \
        qxcbobject.h \
        qxcbscreen.h \
        qxcbwindow.h \
        qxcbwindowsurface.h

LIBS += -lxcb -lxcb-image -lxcb-keysyms

include (../fontdatabases/genericunix/genericunix.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
