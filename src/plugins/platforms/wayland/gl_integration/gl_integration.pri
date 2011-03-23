HEADERS += \
    $$PWD/qwaylandglintegration.h \
    $$PWD/qwaylandglwindowsurface.h

SOURCES += \
    $$PWD/qwaylandglintegration.cpp \
    $$PWD/qwaylandglwindowsurface.cpp

wayland_egl {
    include ($$PWD/wayland_egl/wayland_egl.pri)
}

xpixmap_egl {
    include ($$PWD/xpixmap_egl/xpixmap_egl.pri)
}


