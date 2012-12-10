TARGET = qbbbearer
include(../../qpluginbase.pri)

QT = core network

# Uncomment this to enable debugging output for the plugin
#DEFINES += QBBENGINE_DEBUG

HEADERS += qbbengine.h \
           ../qnetworksession_impl.h \
           ../qbearerengine_impl.h

SOURCES += qbbengine.cpp \
           ../qnetworksession_impl.cpp \
           main.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/bearer
target.path += $$[QT_INSTALL_PLUGINS]/bearer
INSTALLS += target
