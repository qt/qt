TARGET = quikit
include(../../qpluginbase.pri)
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

OBJECTIVE_SOURCES = main.mm \
    quikitintegration.mm \
    quikitwindow.mm \
    quikitscreen.mm \
    quikiteventloop.mm \
    quikitwindowsurface.mm

OBJECTIVE_HEADERS = quikitintegration.h \
    quikitwindow.h \
    quikitscreen.h \
    quikiteventloop.h \
    quikitwindowsurface.h

#add libz for freetype.
LIBS += -lz

#include(../fontdatabases/basicunix/basicunix.pri)
target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
