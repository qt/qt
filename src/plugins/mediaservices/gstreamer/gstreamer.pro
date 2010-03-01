TARGET = gstengine
include(../../qpluginbase.pri)

QT += multimedia

unix:contains(QT_CONFIG, alsa) {
    DEFINES += HAVE_ALSA
    LIBS += -lasound
}

QMAKE_CXXFLAGS += $$QT_CFLAGS_GSTREAMER
LIBS += -lXv $$QT_LIBS_GSTREAMER -lgstinterfaces-0.10 -lgstvideo-0.10 -lgstbase-0.10 -lgstaudio-0.10

# Input
HEADERS += \
    qgstreamermessage.h \
    qgstreamerbushelper.h \
    qgstreamervideooutputcontrol.h \
    qgstreamervideorendererinterface.h \
    qgstreamervideowidget.h \
    qgstreamerserviceplugin.h \
    qgstreamervideoinputdevicecontrol.h \
    qgstreamervideooverlay.h \
    qgstreamervideorenderer.h \
    qgstvideobuffer.h \
    qvideosurfacegstsink.h \
    qx11videosurface.h \
    qgstxvimagebuffer.h


SOURCES += \
    qgstreamermessage.cpp \
    qgstreamerbushelper.cpp \
    qgstreamervideooutputcontrol.cpp \
    qgstreamervideorendererinterface.cpp \
    qgstreamervideowidget.cpp \
    qgstreamerserviceplugin.cpp \
    qgstreamervideoinputdevicecontrol.cpp \
    qgstreamervideooverlay.cpp \
    qgstreamervideorenderer.cpp \
    qgstvideobuffer.cpp \
    qvideosurfacegstsink.cpp \
    qx11videosurface.cpp \
    qgstxvimagebuffer.cpp

include(mediaplayer/mediaplayer.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/mediaservices
target.path = $$[QT_INSTALL_PLUGINS]/plugins/mediaservices
INSTALLS += target
