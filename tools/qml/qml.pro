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
HEADERS += qmlruntime.h \
           proxysettings.h \
           qdeclarativetester.h \
           deviceorientation.h \
           qdeclarativefolderlistmodel.h
SOURCES += main.cpp \
           qmlruntime.cpp \
           proxysettings.cpp \
           qdeclarativetester.cpp \
           qdeclarativefolderlistmodel.cpp
RESOURCES = qmlruntime.qrc
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
    phonon

    contains(QT_CONFIG, xmlpatterns) {
        QT += xmlpatterns
    }
    contains(QT_CONFIG, webkit) {
        QT += webkit 
    }
}
symbian {
#    TARGET.UID3 =
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    INCLUDEPATH += $$QT_SOURCE_TREE/examples/network/qftp/
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    LIBS += -lesock -lcommdb -lconnmon -linsock
    TARGET.CAPABILITY = NetworkServices ReadUserData
}
mac {
    QMAKE_INFO_PLIST=Info_mac.plist
    TARGET=Qml
}
