TARGET = qgstengine
include(../../qpluginbase.pri)

QT += mediaservices

unix:contains(QT_CONFIG, alsa) {
    DEFINES += HAVE_ALSA
    LIBS += -lasound
}

QMAKE_CXXFLAGS += $$QT_CFLAGS_GSTREAMER
LIBS += $$QT_LIBS_GSTREAMER -lgstinterfaces-0.10 -lgstvideo-0.10 -lgstbase-0.10 -lgstaudio-0.10

# Input
HEADERS += \
    qgstreamermessage.h \
    qgstreamerbushelper.h \
    qgstreamervideooutputcontrol.h \
    qgstreamervideorendererinterface.h \    
    qgstreamerserviceplugin.h \
    qgstreamervideoinputdevicecontrol.h \
    qgstreamervideorenderer.h \
    qgstvideobuffer.h \
    qvideosurfacegstsink.h


SOURCES += \
    qgstreamermessage.cpp \
    qgstreamerbushelper.cpp \
    qgstreamervideooutputcontrol.cpp \
    qgstreamervideorendererinterface.cpp \    
    qgstreamerserviceplugin.cpp \
    qgstreamervideoinputdevicecontrol.cpp \
    qgstreamervideorenderer.cpp \
    qgstvideobuffer.cpp \
    qvideosurfacegstsink.cpp


!win32:!embedded:!mac:!symbian:!embedded_lite {
    LIBS += -lXv

    HEADERS += \
        qgstreamervideooverlay.h \
        qgstreamervideowidget.h \
        qx11videosurface.h \
        qgstxvimagebuffer.h

    SOURCES += \
        qgstreamervideooverlay.cpp \
        qgstreamervideowidget.cpp \
        qx11videosurface.cpp \
        qgstxvimagebuffer.cpp
}

include(mediaplayer/mediaplayer.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/mediaservices
target.path = $$[QT_INSTALL_PLUGINS]/mediaservices
INSTALLS += target
