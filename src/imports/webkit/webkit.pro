TARGET  = webkitqmlplugin
TARGETPATH = org/webkit
include(../qimportbase.pri)

QT += webkit declarative
DEFINES += WEBKITQMLPLUGIN_EXPORTS

SOURCES += qdeclarativewebview.cpp plugin.cpp
HEADERS += qdeclarativewebview_p.h \
    qdeclarativewebview_p_p.h \
	webkitqmlplugin_export.h

QTDIR_build:DESTDIR = $$TARGETPATH
target.path = $$TARGETPATH

qmldir.files += $$QT_BUILD_TREE/imports/org/webkit/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
