TEMPLATE = lib
QT += opengl
CONFIG += plugin

TARGET	 = qahiglscreen

target.path = $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS += target

HEADERS	= qwindowsurface_ahigl_p.h \
          qscreenahigl_qws.h

SOURCES	= qwindowsurface_ahigl.cpp \
          qscreenahigl_qws.cpp \
          qscreenahiglplugin.cpp

