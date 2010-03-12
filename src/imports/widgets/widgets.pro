TARGET  = widgets
TARGETPATH = Qt/widgets
include(../qimportbase.pri)

QT += declarative

SOURCES += \
    graphicslayouts.cpp \
    widgets.cpp

HEADERS += \
    graphicswidgets_p.h \
    graphicslayouts_p.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$QT_BUILD_TREE/imports/$$TARGETPATH/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
