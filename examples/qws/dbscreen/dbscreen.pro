TEMPLATE    = lib
CONFIG     += plugin

TARGET      = dbscreen
target.path += $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS    += target

include($$QT_SOURCE_TREE/examples/examplebase.pri)

HEADERS     = dbscreen.h 
SOURCES     = dbscreendriverplugin.cpp \
              dbscreen.cpp 

