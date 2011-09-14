DEFINES += QT_WAYLAND_WINDOWMANAGER_SUPPORT

contains(DEFINES, QT_WAYLAND_WINDOWMANAGER_SUPPORT) {

    HEADERS += \
        $$PWD/qwaylandwindowmanagerintegration.h

    SOURCES += \
        $$PWD/qwaylandwindowmanagerintegration.cpp

    INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/wayland
    HEADERS += \
        $$QT_SOURCE_TREE/src/3rdparty/wayland/qwaylandwindowmanager-client-protocol.h
    SOURCES += \
        $$QT_SOURCE_TREE/src/3rdparty/wayland/wayland-windowmanager-protocol.c
}
