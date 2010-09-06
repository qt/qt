TEMPLATE = lib
QT += gui opengl
INCLUDEPATH += '../'
HEADERS = mgraphicssystem.h mpixmapdata.h mextensions.h
SOURCES = mgraphicssystem.cpp mgraphicssystem.h mgraphicssystemplugin.h mgraphicssystemplugin.cpp mpixmapdata.h mpixmapdata.cpp mextensions.h mextensions.cpp
CONFIG += GLESv2 EGL X11 debug plugin
LIBS += -lGLESv2
TARGET = meegographicssystem

target.path = $$[QT_INSTALL_PLUGINS]/graphicssystems

INSTALLS += target 
