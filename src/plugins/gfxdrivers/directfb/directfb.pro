TARGET = qdirectfbscreen
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/gfxdrivers

# These defines might be necessary if your DirectFB driver doesn't
# support all of the DirectFB API.
#
#DEFINES += QT_DIRECTFB_IMAGECACHE
#DEFINES += QT_NO_DIRECTFB_WM
#DEFINES += QT_NO_DIRECTFB_LAYER
#DEFINES += QT_NO_DIRECTFB_PALETTE
#DEFINES += QT_NO_DIRECTFB_PREALLOCATED
#DEFINES += QT_NO_DIRECTFB_MOUSE
#DEFINES += QT_NO_DIRECTFB_KEYBOARD
#DEFINES += QT_DIRECTFB_TIMING
#DEFINES += QT_NO_DIRECTFB_OPAQUE_DETECTION
#DIRECTFB_DRAWINGOPERATIONS=DRAW_RECTS|DRAW_LINES|DRAW_IMAGE|DRAW_PIXMAP|DRAW_TILED_PIXMAP|STROKE_PATH|DRAW_PATH|DRAW_POINTS|DRAW_ELLIPSE|DRAW_POLYGON|DRAW_TEXT|FILL_PATH|FILL_RECT|DRAW_COLORSPANS
#DEFINES += \"QT_DIRECTFB_WARN_ON_RASTERFALLBACKS=$$DIRECTFB_DRAWINGOPERATIONS\"
#DEFINES += \"QT_DIRECTFB_DISABLE_RASTERFALLBACKS=$$DIRECTFB_DRAWINGOPERATIONS\"

target.path = $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS += target

HEADERS = qdirectfbscreen.h \
          qdirectfbwindowsurface.h \
          qdirectfbpaintengine.h \
          qdirectfbpaintdevice.h \
          qdirectfbpixmap.h \
          qdirectfbkeyboard.h \
          qdirectfbmouse.h

SOURCES = qdirectfbscreen.cpp \
          qdirectfbscreenplugin.cpp \
          qdirectfbwindowsurface.cpp \
          qdirectfbpaintengine.cpp \
          qdirectfbpaintdevice.cpp \
          qdirectfbpixmap.cpp \
          qdirectfbkeyboard.cpp \
          qdirectfbmouse.cpp

QMAKE_CXXFLAGS += $$QT_CFLAGS_DIRECTFB
LIBS += $$QT_LIBS_DIRECTFB
DEFINES += $$QT_DEFINES_DIRECTFB
