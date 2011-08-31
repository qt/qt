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

HEADERS = quikitsoftwareinputhandler.h \
    qcoretextfontdatabase.h

SOURCES += \
    qcoretextfontdatabase.cpp

#needed for qcoretextfontengine even if it's not used
INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/harfbuzz/src

#add libz for freetype.
LIBS += -lz

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target


