TARGET = blackberry
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms
QT += opengl

blackberry {
    # Unomment this to enable screen event handling
    # through a dedicated thread.
    # DEFINES += QQNX_SCREENEVENTTHREAD
} else {
    DEFINES += QBB_SCREENEVENTTHREAD
}

SOURCES =   main.cpp \
            qbbbuffer.cpp \
            qbbscreeneventthread.cpp \
            qbbglcontext.cpp \
            qbbglwindowsurface.cpp \
            qbbinputcontext.cpp \
            qbbintegration.cpp \
            qbbnavigatoreventhandler.cpp \
            qbbnavigatoreventnotifier.cpp \
            qbbscreen.cpp \
            qbbwindow.cpp \
            qbbrasterwindowsurface.cpp \
            qbbvirtualkeyboardpps.cpp \
            qbbclipboard.cpp \
            qbblocalethread.cpp \
            qbbrootwindow.cpp \
            qbbscreeneventhandler.cpp \
            qbbabstractvirtualkeyboard.cpp \
            qbbnativeinterface.cpp \
            qbbbuttoneventnotifier.cpp \
            qbbcursor.cpp

HEADERS =   qbbbuffer.h \
            qbbscreeneventthread.h \
            qbbinputcontext.h \
            qbbintegration.h \
            qbbnavigatoreventhandler.h \
            qbbnavigatoreventnotifier.h \
            qbbglcontext.h \
            qbbglwindowsurface.h \
            qbbscreen.h \
            qbbwindow.h \
            qbbrasterwindowsurface.h \
            qbbvirtualkeyboardpps.h \
            qbbclipboard.h \
            qbblocalethread.h \
            qbbrootwindow.h \
            qbbscreeneventhandler.h \
            qbbabstractvirtualkeyboard.h \
            qbbnativeinterface.h \
            qbbbuttoneventnotifier.h \
            qbbcursor.h

blackberry {
    SOURCES += qbbbpseventfilter.cpp \
               qbbvirtualkeyboardbps.cpp

    HEADERS += qbbbpseventfilter.h \
               qbbvirtualkeyboardbps.h
}

QMAKE_CXXFLAGS += -I./private

LIBS += -lpps -lscreen -lEGL -lclipboard

include (../eglconvenience/eglconvenience.pri)
include (../fontdatabases/genericunix/genericunix.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
