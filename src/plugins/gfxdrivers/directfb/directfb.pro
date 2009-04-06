TARGET = qdirectfbscreen
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/gfxdrivers

# These defines might be necessary if your DirectFB driver doesn't 
# support all of the DirectFB API.
#
#DEFINES += QT_NO_DIRECTFB_WM
#DEFINES += QT_NO_DIRECTFB_LAYER
#DEFINES += QT_NO_DIRECTFB_PALETTE
#DEFINES += QT_NO_DIRECTFB_PREALLOCATED
#DEFINES += QT_NO_DIRECTFB_MOUSE
#DEFINES += QT_NO_DIRECTFB_KEYBOARD
#DEFINES += QT_DIRECTFB_TIMING

target.path = $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS += target

HEADERS	= \
	qdirectfbscreen.h \
	qdirectfbsurface.h \
	qdirectfbpaintengine.h \
        qdirectfbpaintdevice.h \
	qdirectfbpixmap.h \
	qdirectfbkeyboard.h \
	qdirectfbmouse.h

SOURCES	= \
	qdirectfbscreen.cpp \
	qdirectfbscreenplugin.cpp \
	qdirectfbsurface.cpp \
	qdirectfbpaintengine.cpp \
        qdirectfbpaintdevice.cpp \
	qdirectfbpixmap.cpp \
	qdirectfbkeyboard.cpp \
	qdirectfbmouse.cpp

QMAKE_CXXFLAGS += $$QT_CFLAGS_DIRECTFB
LIBS += $$QT_LIBS_DIRECTFB

