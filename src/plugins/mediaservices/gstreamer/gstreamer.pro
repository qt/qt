TARGET = gstengine
include(../../qpluginbase.pri)

QT += multimedia

unix:contains(QT_CONFIG, alsa) {
    DEFINES += HAVE_ALSA
    LIBS += -lasound
}

LIBS += -lXv

CONFIG += link_pkgconfig

PKGCONFIG += \
    gstreamer-0.10 \
    gstreamer-base-0.10 \
    gstreamer-interfaces-0.10 \
    gstreamer-audio-0.10 \
    gstreamer-video-0.10

# Input
HEADERS += \
    qgstreamermessage.h \
    qgstreamerbushelper.h \
    qgstreamervideooutputcontrol.h \
    qgstreamervideorendererinterface.h \
    qgstreamervideowidget.h \
    qgstreamerserviceplugin.h \
#    qgstreameraudioinputendpointselector.h \
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
#    qgstreameraudioinputendpointselector.cpp \
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
