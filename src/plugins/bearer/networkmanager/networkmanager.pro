TARGET = qnmbearer
include(../../qpluginbase.pri)

QT += network dbus

DEFINES += BEARER_ENGINE BACKEND_NM

HEADERS += qnmdbushelper.h \
           qnetworkmanagerservice.h \
           qnmwifiengine.h

SOURCES += main.cpp \
           qnmdbushelper.cpp \
           qnetworkmanagerservice.cpp \
           qnmwifiengine.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/bearer
target.path += $$[QT_INSTALL_PLUGINS]/bearer
INSTALLS += target
