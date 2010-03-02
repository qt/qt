TARGET = qicdbearer
include(../../qpluginbase.pri)

QT += network dbus

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0 dbus-glib-1 gconf-2.0 osso-ic conninet

HEADERS += qicdengine.h \
           monitor.h \
           qnetworksession_impl.h

SOURCES += main.cpp \
           qicdengine.cpp \
           monitor.cpp \
           qnetworksession_impl.cpp

#DEFINES += BEARER_MANAGEMENT_DEBUG

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/bearer
target.path += $$[QT_INSTALL_PLUGINS]/bearer
INSTALLS += target
