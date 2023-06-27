TARGET  = qgif
include(../../qpluginbase.pri)

include(../../../gui/image/qgifhandler.pri)
SOURCES += $$PWD/main.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
