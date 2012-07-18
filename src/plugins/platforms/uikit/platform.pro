TARGET = quikit
include(../../qpluginbase.pri)
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

QT += opengl declarative

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

HEADERS = quikitsoftwareinputhandler.h

include(../fontdatabases/coretext/coretext.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
