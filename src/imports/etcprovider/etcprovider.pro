TARGET  = qmletcproviderplugin
TARGETPATH = Qt/labs/etcprovider
include(../qimportbase.pri)
!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

QT += declarative opengl

SOURCES += qetcprovider.cpp plugin.cpp
HEADERS += qetcprovider.h plugin.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

OTHER_FILES +=
