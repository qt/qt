TEMPLATE = app
CONFIG += qt \
    uic
DESTDIR = ../../bin
QT += declarative \
    script \
    network \
    sql

contains(QT_CONFIG, opengl) {
    QT += opengl
    DEFINES += GL_SUPPORTED
}

# Input
HEADERS += qmlviewer.h \
           proxysettings.h \
           qfxtester.h \
           deviceorientation.h \
           qmlfolderlistmodel.h
SOURCES += main.cpp \
           qmlviewer.cpp \
           proxysettings.cpp \
           qfxtester.cpp \
           qmlfolderlistmodel.cpp
RESOURCES = qmlviewer.qrc
maemo5 {
    SOURCES += deviceorientation_maemo.cpp
} else {
    SOURCES += deviceorientation.cpp
}
FORMS = recopts.ui \
    proxysettings.ui
INCLUDEPATH += ../../include/QtDeclarative
INCLUDEPATH += ../../src/declarative/util
INCLUDEPATH += ../../src/declarative/graphicsitems
include(../shared/deviceskin/deviceskin.pri)
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

wince* {
QT += scripttools \
    xml \
    xmlpatterns \
    phonon

    contains(QT_CONFIG, webkit) {
        QT += webkit 
    }
}
symbian {
#    TARGET.UID3 =
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    HEADERS += $$QT_SOURCE_TREE/examples/network/qftp/sym_iap_util.h
    LIBS += -lesock  -lconnmon -linsock
    TARGET.CAPABILITY = "All -TCB"
}
