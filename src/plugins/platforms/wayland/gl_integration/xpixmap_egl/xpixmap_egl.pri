
LIBS += -lX11 -lXext -lEGL

HEADERS += \
    gl_integration/xpixmap_egl/qwaylandxpixmapeglintegration.h \
    gl_integration/xpixmap_egl/qwaylandxpixmapwindow.h \
    gl_integration/xpixmap_egl/qwaylandxpixmapeglcontext.h \
    $$PWD/../../../eglconvenience/qeglconvenience.h

SOURCES += \
    gl_integration/xpixmap_egl/qwaylandxpixmapeglintegration.cpp \
    gl_integration/xpixmap_egl/qwaylandxpixmapwindow.cpp \
    gl_integration/xpixmap_egl/qwaylandxpixmapeglcontext.cpp \
    $$PWD/../../../eglconvenience/qeglconvenience.cpp
