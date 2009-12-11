TARGET = qnmbearer
include(../../qpluginbase.pri)

QT += network dbus

DEFINES += BACKEND_NM

HEADERS += qnmdbushelper.h \
           qnetworkmanagerservice.h \
           qnetworkmanagerengine.h

SOURCES += main.cpp \
           qnmdbushelper.cpp \
           qnetworkmanagerservice.cpp \
           qnetworkmanagerengine.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/bearer
target.path += $$[QT_INSTALL_PLUGINS]/bearer
INSTALLS += target
