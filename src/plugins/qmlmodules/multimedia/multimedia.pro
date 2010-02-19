TARGET  = multimedia
include(../../qpluginbase.pri)

QT += multimedia declarative

SOURCES += multimedia.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/qmlmodules
target.path = $$[QT_INSTALL_PLUGINS]/plugins/qmlmodules
INSTALLS += target

