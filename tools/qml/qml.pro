TEMPLATE = app
CONFIG += qt uic
DESTDIR = ../../bin

include(qml.pri)

SOURCES += main.cpp 

INCLUDEPATH += ../../include/QtDeclarative
INCLUDEPATH += ../../src/declarative/util
INCLUDEPATH += ../../src/declarative/graphicsitems

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
