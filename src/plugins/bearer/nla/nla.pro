TARGET = qnlabearer
include(../../qpluginbase.pri)

QT += network

!wince* {
    LIBS += -lWs2_32
} else {
    LIBS += -lWs2
}

DEFINES += BEARER_ENGINE

HEADERS += qnlaengine.h \
           ../platformdefs_win.h
SOURCES += qnlaengine.cpp main.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/bearer
target.path += $$[QT_INSTALL_PLUGINS]/bearer
INSTALLS += target
