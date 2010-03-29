TARGET = qt7
include(../../qpluginbase.pri)

QT += opengl multimedia

LIBS += -framework AppKit -framework AudioUnit \
        -framework AudioToolbox -framework CoreAudio \
        -framework QuartzCore -framework QTKit

# The Quicktime framework is only awailable for 32-bit builds, so we
# need to check for this before linking against it.
# QMAKE_MAC_XARCH is not awailable on Tiger, but at the same time,
# we never build for 64-bit architechtures on Tiger either:
contains(QMAKE_MAC_XARCH, no) {
    LIBS += -framework QuickTime
} else {
    LIBS += -Xarch_i386 -framework QuickTime -Xarch_ppc -framework QuickTime
}

HEADERS += \
    qt7backend.h \
    qt7videooutputcontrol.h \
    qt7movieviewoutput.h \
    qt7movievideowidget.h \
    qt7movieviewrenderer.h \
    qt7serviceplugin.h \
    qt7movierenderer.h \
    qt7ciimagevideobuffer.h \
    qcvdisplaylink.h

OBJECTIVE_SOURCES += \
    qt7backend.mm \
    qt7serviceplugin.mm \
    qt7movieviewoutput.mm \
    qt7movievideowidget.mm \
    qt7movieviewrenderer.mm \
    qt7movierenderer.mm \
    qt7videooutputcontrol.mm \
    qt7ciimagevideobuffer.mm \
    qcvdisplaylink.mm

include(mediaplayer/mediaplayer.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/mediaservices
target.path = $$[QT_INSTALL_PLUGINS]/mediaservices
INSTALLS += target

