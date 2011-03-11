TARGET  = qmlinputcontextplugin
TARGETPATH = Qt/labs/inputcontext
include(../qimportbase.pri)

QT += declarative script

SOURCES += \
        declarativeinputcontext.cpp \
        inputcontextfilter.cpp \
        inputcontextmodule.cpp \
        plugin.cpp

HEADERS  += \
        declarativeinputcontext.h \
        inputcontextfilter.h \
        inputcontextmodule.h

OTHER_FILES = \
        qmldir

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

symbian:{
    TARGET.UID3 = 0x20031E91

    isEmpty(DESTDIR):importFiles.files = qmlinputcontextplugin{QT_LIBINFIX}.dll qmldir
    else:importFiles.files = $$DESTDIR/qmlinputcontextplugin$${QT_LIBINFIX}.dll qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH

    DEPLOYMENT = importFiles
}

INSTALLS += target qmldir

