TARGET  = widgets
include(../../qpluginbase.pri)

QT += declarative

SOURCES += \
    graphicslayouts.cpp \
    widgets.cpp

HEADERS += \
    graphicswidgets_p.h \
    graphicslayouts_p.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/Qt/widgets
target.path = $$[QT_INSTALL_IMPORTS]/Qt/widgets

qmldir.files += $$QT_BUILD_TREE/imports/Qt/widgets/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/Qt/widgets

INSTALLS += target qmldir
