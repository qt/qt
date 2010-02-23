TARGET  = widgets
include(../../qpluginbase.pri)

QT += declarative

SOURCES += \
    graphicslayouts.cpp \
    widgets.cpp

HEADERS += \
    graphicswidgets_p.h \
    graphicslayouts_p.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/qmlmodules
target.path = $$[QT_INSTALL_PLUGINS]/plugins/qmlmodules
