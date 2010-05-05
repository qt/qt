TARGET = qmmfengine
QT +=  multimedia mediaservices

load(data_caging_paths)

include (../../qpluginbase.pri)
include(mediaplayer/mediaplayer.pri)

HEADERS += s60mediaserviceplugin.h \
           s60videooutputcontrol.h

SOURCES += s60mediaserviceplugin.cpp \
           s60videooutputcontrol.cpp

contains(S60_VERSION, 3.2)|contains(S60_VERSION, 3.1) {
    DEFINES += PRE_S60_50_PLATFORM
}

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
symbian-abld:INCLUDEPATH += $$QT_BUILD_TREE/include/QtWidget/private

# This is needed for having the .qtplugin file properly created on Symbian.
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/mediaservices
target.path += $$[QT_INSTALL_PLUGINS]/mediaservices
INSTALLS += target

TARGET.UID3=0x20021318
