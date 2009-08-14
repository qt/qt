TEMPLATE = app
CONFIG += qt \
    uic
DESTDIR = ../../bin
QT += declarative \
    script \
    network \
    sql

# Input
HEADERS += qmlviewer.h \
    proxysettings.h
SOURCES += main.cpp \
    qmlviewer.cpp \
    proxysettings.cpp
FORMS = recopts.ui \
    proxysettings.ui
include($$QT_SOURCE_TREE/tools/shared/deviceskin/deviceskin.pri)
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

wince* {
QT += scripttools \
    xml \
    xmlpatterns \
    webkit \
    phonon
}
