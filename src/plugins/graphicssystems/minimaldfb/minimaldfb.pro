TARGET = qminimaldfb
include(../../qpluginbase.pri)
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

system(pkg-config --exists directfb) {
    DIRECTFB_CFLAGS = $$system(pkg-config --cflags directfb)
    DIRECTFB_LIBS = $$system(pkg-config --libs directfb)
} else {
    directfb_config = $$SYSTEM(which directfb-config)
    !isEmpty(directfb_config) {
        DIRECTFB_CFLAGS = $$system(directfb-config --cflags)
        DIRECTFB_LIBS = $$system(directfb-config --libs)
    }
}

QMAKE_CXXFLAGS += $$DIRECTFB_CFLAGS
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
