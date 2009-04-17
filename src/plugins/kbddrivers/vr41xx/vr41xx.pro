TARGET = qvr41xxkbddriver
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/kbddrivers
target.path = $$[QT_INSTALL_PLUGINS]/kbddrivers
INSTALLS += target

DEFINES += QT_QWS_KBD_VR41XX

HEADERS	= $$QT_SOURCE_TREE/src/gui/embedded/qkbdvr41xx_qws.h

SOURCES	= main.cpp \
	$$QT_SOURCE_TREE/src/gui/embedded/qkbdvr41xx_qws.cpp

