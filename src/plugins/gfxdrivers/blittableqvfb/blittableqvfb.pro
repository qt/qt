TARGET = qscreenblittablevfb
include(../../qpluginbase.pri)

DEFINES	+= QT_QWS_QVFB QT_QWS_MOUSE_QVFB QT_QWS_KBD_QVFB

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/gfxdrivers

HEADERS = qgraphicssystem_blittableqvfb.h \
          qscreen_blittableqvfb.h \
          qwindowsurface_qwsblittable.h \
          qblittable_image.h

SOURCES = main.cpp \
          qwindowsurface_qwsblittable.cpp

#include qvfb sources
HEADERS += \
	$$QT_SOURCE_TREE/src/gui/embedded/qscreenvfb_qws.h \
	$$QT_SOURCE_TREE/src/gui/embedded/qkbdvfb_qws.h \
	$$QT_SOURCE_TREE/src/gui/embedded/qmousevfb_qws.h

SOURCES	+= \
	$$QT_SOURCE_TREE/src/gui/embedded/qscreenvfb_qws.cpp \
	$$QT_SOURCE_TREE/src/gui/embedded/qkbdvfb_qws.cpp \
	$$QT_SOURCE_TREE/src/gui/embedded/qmousevfb_qws.cpp

target.path += $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS += target
