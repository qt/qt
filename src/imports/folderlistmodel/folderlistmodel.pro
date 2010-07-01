TARGET  = qmlfolderlistmodelplugin
TARGETPATH = Qt/labs/folderlistmodel
include(../qimportbase.pri)

QT += declarative script

SOURCES += qdeclarativefolderlistmodel.cpp plugin.cpp
HEADERS += qdeclarativefolderlistmodel.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
else:DESTDIR = .
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

symbian:{
    TARGET.UID3 = 0x20021320
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    
    importFiles.sources = $$DESTDIR/qmlfolderlistmodelplugin$${QT_LIBINFIX}.dll qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    
    DEPLOYMENT = importFiles
}

INSTALLS += target qmldir
