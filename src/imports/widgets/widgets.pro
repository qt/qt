TARGET  = widgets
TARGETPATH = Qt/widgets
include(../qimportbase.pri)

QT += declarative

SOURCES += \
    graphicslayouts.cpp \
    widgets.cpp

HEADERS += \
    graphicslayouts_p.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

symbian:{
    load(data_caging_paths)
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    
    importFiles.sources = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH/widgets.dll \
    qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    
    DEPLOYMENT = importFiles
}

INSTALLS += target qmldir
