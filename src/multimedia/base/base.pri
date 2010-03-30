
QT += network
contains(QT_CONFIG, opengl):QT += opengl

HEADERS += \
    $$PWD/qmediaresource.h \
    $$PWD/qmediacontent.h \
    $$PWD/qmediaobject.h \
    $$PWD/qmediaobject_p.h \
    $$PWD/qmediapluginloader_p.h \
    $$PWD/qmediaservice.h \
    $$PWD/qmediaservice_p.h \
    $$PWD/qmediaserviceprovider.h \
    $$PWD/qmediaserviceproviderplugin.h \
    $$PWD/qmediacontrol.h \
    $$PWD/qmediacontrol_p.h \
    $$PWD/qmetadatacontrol.h \
    $$PWD/qvideooutputcontrol.h \
    $$PWD/qvideowindowcontrol.h \
    $$PWD/qvideorenderercontrol.h \
    $$PWD/qvideodevicecontrol.h \
    $$PWD/qvideowidgetcontrol.h \
    $$PWD/qvideowidget.h \
    $$PWD/qvideowidget_p.h \
    $$PWD/qgraphicsvideoitem.h \
    $$PWD/qmediaplaylistcontrol.h \
    $$PWD/qmediaplaylist.h \
    $$PWD/qmediaplaylist_p.h \
    $$PWD/qmediaplaylistprovider.h \
    $$PWD/qmediaplaylistprovider_p.h \
    $$PWD/qmediaplaylistioplugin.h \
    $$PWD/qlocalmediaplaylistprovider.h \
    $$PWD/qmediaplaylistnavigator.h \
    $$PWD/qpaintervideosurface_p.h \
    $$PWD/qmediatimerange.h \
    $$PWD/qtmedianamespace.h

SOURCES += \
    $$PWD/qmediaresource.cpp \
    $$PWD/qmediacontent.cpp \
    $$PWD/qmediaobject.cpp \
    $$PWD/qmediapluginloader.cpp \
    $$PWD/qmediaservice.cpp \
    $$PWD/qmediaserviceprovider.cpp \
    $$PWD/qmediacontrol.cpp \
    $$PWD/qmetadatacontrol.cpp \
    $$PWD/qvideooutputcontrol.cpp \
    $$PWD/qvideowindowcontrol.cpp \
    $$PWD/qvideorenderercontrol.cpp \
    $$PWD/qvideodevicecontrol.cpp \
    $$PWD/qvideowidgetcontrol.cpp \
    $$PWD/qvideowidget.cpp \
    $$PWD/qgraphicsvideoitem.cpp \
    $$PWD/qmediaplaylistcontrol.cpp \
    $$PWD/qmediaplaylist.cpp \
    $$PWD/qmediaplaylistprovider.cpp \
    $$PWD/qmediaplaylistioplugin.cpp \
    $$PWD/qlocalmediaplaylistprovider.cpp \
    $$PWD/qmediaplaylistnavigator.cpp \
    $$PWD/qpaintervideosurface.cpp \
    $$PWD/qmediatimerange.cpp

mac {
    HEADERS += $$PWD/qpaintervideosurface_mac_p.h
    OBJECTIVE_SOURCES += $$PWD/qpaintervideosurface_mac.mm

    LIBS += -framework AppKit -framework QuartzCore -framework QTKit

}
