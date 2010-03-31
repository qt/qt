TARGET = qdirectfb
include(../../qpluginbase.pri)
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

isEmpty(DIRECTFB_LIBS) {
    DIRECTFB_LIBS = -ldirectfb -lfusion -ldirect -lpthread
}
isEmpty(DIRECTFB_INCLUDEPATH) {
    DIRECTFB_INCLUDEPATH = /usr/include/directfb
}

INCLUDEPATH += $$DIRECTFB_INCLUDEPATH
LIBS += $$DIRECTFB_LIBS

SOURCES = main.cpp \
    qplatformintegration_directfb.cpp \
    qwindowsurface_directfb.cpp \
    qblitter_directfb.cpp \
    qdirectfbconvenience.cpp \
    qdirectfbinput.cpp \
    qdirectfbcursor.cpp
HEADERS = qplatformintegration_directfb.h \
    qwindowsurface_directfb.h \
    qblitter_directfb.h \
    qdirectfbconvenience.h \
    qdirectfbinput.h \
    qdirectfbcursor.h
target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
