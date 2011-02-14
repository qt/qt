TARGET = qtestlite

include(../../qpluginbase.pri)
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES = \
        main.cpp \
        qtestliteintegration.cpp \
        qtestlitewindowsurface.cpp \
        qtestlitewindow.cpp \
        qtestlitecursor.cpp \
        qtestlitescreen.cpp \
        qtestlitekeyboard.cpp \
        qtestliteclipboard.cpp \
        qtestlitemime.cpp \
        qtestlitestaticinfo.cpp

HEADERS = \
        qtestliteintegration.h \
        qtestlitewindowsurface.h \
        qtestlitewindow.h \
        qtestlitecursor.h \
        qtestlitescreen.h \
        qtestlitekeyboard.h \
        qtestliteclipboard.h \
        qtestlitemime.h \
        qtestlitestaticinfo.h

LIBS += -lX11 -lXext

mac {
    LIBS += -L/usr/X11/lib -lz -framework Carbon
}

include (../fontdatabases/genericunix/genericunix.pri)

contains(QT_CONFIG, opengl) {
    QT += opengl
    !contains(QT_CONFIG, opengles2) {
        HEADERS += qglxintegration.h
        SOURCES += qglxintegration.cpp
    } else { # There is no easy way to detect if we'r suppose to use glx or not
        HEADERS += \
            ../eglconvenience/qeglplatformcontext.h \
            ../eglconvenience/qeglconvenience.h \
            qtestliteeglintegration.h

        SOURCES += \
            ../eglconvenience/qeglplatformcontext.cpp \
            ../eglconvenience/qeglconvenience.cpp \
            qtestliteeglintegration.cpp
        LIBS += -lEGL
    }
}

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
