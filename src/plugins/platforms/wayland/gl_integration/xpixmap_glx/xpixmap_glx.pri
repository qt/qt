include (../../../glxconvenience/glxconvenience.pri)
HEADERS += \
    $$PWD/qwaylandxpixmapglxintegration.h \
    gl_integration/xpixmap_glx/qwaylandxpixmapglxwindow.h \
    gl_integration/xpixmap_glx/qwaylandxpixmapglxcontext.h

SOURCES += \
    $$PWD/qwaylandxpixmapglxintegration.cpp \
    gl_integration/xpixmap_glx/qwaylandxpixmapglxwindow.cpp \
    gl_integration/xpixmap_glx/qwaylandxpixmapglxcontext.cpp

LIBS += -lX11
