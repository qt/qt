TARGET = qminimaldfb
include(../../qpluginbase.pri)
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

isEmpty(DIRECTFB_LIBS) {
    DIRECTFB_LIBS = -ldirectfb -lfusion -ldirect -lpthread
}
isEmpty(DIRECTFB_INCLUDEPATH) {
    DIRECTFB_INCLUDEPATH = /usr/include/directfb
}

INCLUDEPATH += $$DIRECTFB_INCLUDEPATH
LIBS += $$DIRECTFB_LIBS

SOURCES = main.cpp \
    qgraphicssystem_minimaldfb.cpp \
    qwindowsurface_minimaldfb.cpp \
    qblitter_directfb.cpp \
    qdirectfbconvenience.cpp \
    qdirectfbinput.cpp \
    qdirectfbcursor.cpp
HEADERS = qgraphicssystem_minimaldfb.h \
    qwindowsurface_minimaldfb.h \
    qblitter_directfb.h \
    qdirectfbconvenience.h \
    qdirectfbinput.h \
    qdirectfbcursor.h
target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target
