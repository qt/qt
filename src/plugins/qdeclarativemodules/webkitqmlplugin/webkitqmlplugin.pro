TARGET  = webkitqmlplugin
include(../../qpluginbase.pri)

contains(QT_CONFIG, webkit) {
    QT += webkit declarative

    SOURCES += qdeclarativewebview.cpp plugin.cpp
    HEADERS += qdeclarativewebview_p.h
    HEADERS += qdeclarativewebview_p_p.h

    QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/org/webkit
    target.path = $$[QT_INSTALL_IMPORTS]/org/webkit

    qmldir.files += $$QT_BUILD_TREE/imports/org/webkit/qmldir
    qmldir.path +=  $$[QT_INSTALL_IMPORTS]/org/webkit

    INSTALLS += target qmldir
}
