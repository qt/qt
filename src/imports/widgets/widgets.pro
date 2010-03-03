TARGET  = widgets
TARGETPATH = $$[QT_INSTALL_IMPORTS]/Qt/widgets
include(../qimportbase.pri)

QT += declarative

SOURCES += \
    graphicslayouts.cpp \
    widgets.cpp

HEADERS += \
    graphicswidgets_p.h \
    graphicslayouts_p.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/Qt/widgets
target.path = $$TARGETPATH

# install qmldir file
qmldir.files += qmldir
qmldir.path = $$TARGETPATH

INSTALLS += target qmldir
