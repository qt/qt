TARGET = qopenkodegraphicssystem
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/graphicssystems

SOURCES = main.cpp qgraphicssystem_openkode.cpp qwindowsurface_openkode.cpp
HEADERS = qgraphicssystem_openkode.h qwindowsurface_openkode.h

target.path += $$[QT_INSTALL_PLUGINS]/graphicssystems
INSTALLS += target

# openkode specific stuff
INCLUDEPATH += $(OPENKODE_DIR)/include
LIBS += $${QMAKE_RPATH}/$(OPENKODE_DIR)/lib-target -L$(OPENKODE_DIR)/lib-target -lKD -lEGL -lGLESv2
