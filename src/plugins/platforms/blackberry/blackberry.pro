TARGET = blackberry
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms
QT += opengl

SOURCES =   main.cpp \
            qbbbuffer.cpp \
            qbbeventthread.cpp \
            qbbglcontext.cpp \
            qbbglwindowsurface.cpp \
            qbbinputcontext.cpp \
            qbbintegration.cpp \
            qbbnavigatoreventhandler.cpp \
            qbbnavigatoreventnotifier.cpp \
            qbbscreen.cpp \
            qbbwindow.cpp \
            qbbrasterwindowsurface.cpp \
            qbbvirtualkeyboard.cpp \
            qbbclipboard.cpp \
            qbblocalethread.cpp \
            qbbrootwindow.cpp \
            qbbscreeneventhandler.cpp \
            qbbabstractvirtualkeyboard.cpp \
            qbbnativeinterface.cpp

HEADERS =   qbbbuffer.h \
            qbbeventthread.h \
            qbbinputcontext.h \
            qbbintegration.h \
            qbbnavigatoreventhandler.h \
            qbbnavigatoreventnotifier.h \
            qbbglcontext.h \
            qbbglwindowsurface.h \
            qbbscreen.h \
            qbbwindow.h \
            qbbrasterwindowsurface.h \
            qbbvirtualkeyboard.h \
            qbbclipboard.h \
            qbblocalethread.h \
            qbbrootwindow.h \
            qbbscreeneventhandler.h \
            qbbabstractvirtualkeyboard.h \
            qbbnativeinterface.h

QMAKE_CXXFLAGS += -I./private

LIBS += -lpps -lscreen -lEGL -lclipboard

include (../eglconvenience/eglconvenience.pri)
include (../fontdatabases/genericunix/genericunix.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
