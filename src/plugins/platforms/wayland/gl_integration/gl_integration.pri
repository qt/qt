HEADERS += \
    $$PWD/qwaylandglintegration.h \
    $$PWD/qwaylandglwindowsurface.h

SOURCES += \
    $$PWD/qwaylandglintegration.cpp \
    $$PWD/qwaylandglwindowsurface.cpp

wayland_egl {
    include ($$PWD/wayland_egl/wayland_egl.pri)
}

readback_egl {
    include ($$PWD/readback_egl/readback_egl.pri)
}

readback_glx {
    include ($$PWD/readback_glx/readback_glx.pri)
}

xcomposite_glx {
    include ($$PWD/xcomposite_glx/xcomposite_glx.pri)
}

xcomposite_egl {
    include ($$PWD/xcomposite_egl/xcomposite_egl.pri)
}
