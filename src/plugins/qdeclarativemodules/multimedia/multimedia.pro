TARGET  = multimedia
include(../../qpluginbase.pri)

QT += multimedia declarative

SOURCES += multimedia.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/Qt/multimedia
target.path = $$[QT_INSTALL_IMPORTS]/Qt/multimedia

qmldir.files += $$QT_BUILD_TREE/imports/Qt/multimedia/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/Qt/multimedia

INSTALLS += target qmldir

