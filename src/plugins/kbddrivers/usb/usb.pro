TARGET = qusbkbddriver
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/kbddrivers
target.path = $$[QT_INSTALL_PLUGINS]/kbddrivers
INSTALLS += target

DEFINES += QT_QWS_KBD_USB

HEADERS	= $$QT_SOURCE_TREE/src/gui/embedded/qkbdusb_qws.h

SOURCES	= main.cpp \
	$$QT_SOURCE_TREE/src/gui/embedded/qkbdusb_qws.cpp

