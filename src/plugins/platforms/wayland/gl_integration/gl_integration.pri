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

xpixmap_glx {
    include ($$PWD/xpixmap_glx/xpixmap_glx.pri)
}

xcomposite_glx {
    include ($$PWD/xcomposite_glx/xcomposite_glx.pri)
}
