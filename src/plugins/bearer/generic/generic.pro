TARGET = qgenericbearer
include(../../qpluginbase.pri)

QT += network

HEADERS += qgenericengine.h \
           ../qnetworksession_impl.h \
           ../qnetworksessionengine_impl.h \
           ../platformdefs_win.h
SOURCES += qgenericengine.cpp \
           ../qnetworksession_impl.cpp \
           main.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/bearer
target.path += $$[QT_INSTALL_PLUGINS]/bearer
INSTALLS += target
