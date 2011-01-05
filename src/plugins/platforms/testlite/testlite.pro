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
    HEADERS += qglxintegration.h
    SOURCES += qglxintegration.cpp
}

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
