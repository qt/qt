TARGET  = multimedia
TARGETPATH = $$[QT_INSTALL_IMPORTS]/Qt/multimedia
include(../qimportbase.pri)

QT += multimedia declarative

SOURCES += multimedia.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/Qt/multimedia
target.path = $$TARGETPATH

qmldir.files += $$QT_BUILD_TREE/imports/Qt/multimedia/qmldir
qmldir.path +=  $$TARGETPATH

INSTALLS += target qmldir
