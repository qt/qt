TARGET = qsl5000kbddriver
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/kbddrivers
target.path = $$[QT_INSTALL_PLUGINS]/kbddrivers
INSTALLS += target

DEFINES += QT_QWS_KBD_SL5000 QT_QWS_KBD_TTY

HEADERS	= $$QT_SOURCE_TREE/src/gui/embedded/qkbdsl5000_qws.h \
          $$QT_SOURCE_TREE/src/gui/embedded/qkbdtty_qws.h

SOURCES	= main.cpp \
	$$QT_SOURCE_TREE/src/gui/embedded/qkbdsl5000_qws.cpp \
        $$QT_SOURCE_TREE/src/gui/embedded/qkbdtty_qws.cpp

