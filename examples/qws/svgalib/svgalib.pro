TEMPLATE = lib
CONFIG += plugin

LIBS += -lvgagl -lvga

TARGET = svgalibscreen
target.path = $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS += target

include($$QT_SOURCE_TREE/examples/examplebase.pri)

HEADERS	= svgalibscreen.h \
          svgalibpaintengine.h \
          svgalibsurface.h \
          svgalibpaintdevice.h
SOURCES	= svgalibscreen.cpp \
          svgalibpaintengine.cpp \
          svgalibsurface.cpp \
          svgalibpaintdevice.cpp \
          svgalibplugin.cpp

