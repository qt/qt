TARGET  = multimedia
include(../../qpluginbase.pri)

QT += multimedia declarative

SOURCES += multimedia.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/qdeclarativemodules
target.path = $$[QT_INSTALL_PLUGINS]/plugins/qdeclarativemodules
INSTALLS += target

