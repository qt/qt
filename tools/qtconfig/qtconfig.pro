TEMPLATE = app
CONFIG   += qt warn_on x11
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
LANGUAGE = C++

SOURCES        += colorbutton.cpp main.cpp previewframe.cpp previewwidget.cpp mainwindow.cpp paletteeditoradvanced.cpp
HEADERS        += colorbutton.h previewframe.h previewwidget.h mainwindow.h paletteeditoradvanced.h

FORMS        = mainwindow.ui paletteeditoradvanced.ui previewwidget.ui
RESOURCES    = qtconfig.qrc

PROJECTNAME        = Qt Configuration
TARGET             = qtconfig
DESTDIR            = ../../bin

target.path=$$[QT_INSTALL_BINS]
INSTALLS        += target
INCLUDEPATH     += .
DBFILE          = qtconfig.db
