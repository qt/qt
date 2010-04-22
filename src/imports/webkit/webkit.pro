TARGET  = webkitqmlplugin
TARGETPATH = org/webkit
include(../qimportbase.pri)

QT += webkit declarative

SOURCES += qdeclarativewebview.cpp plugin.cpp
HEADERS += qdeclarativewebview_p.h \
    qdeclarativewebview_p_p.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

symbian:{
    load(data_caging_paths)
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    
    importFiles.sources = webkitqmlplugin.dll qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    
    DEPLOYMENT = importFiles
}

INSTALLS += target qmldir
