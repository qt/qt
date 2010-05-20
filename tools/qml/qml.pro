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
    QT += xml

    contains(QT_CONFIG, scripttools) {
        QT += scripttools
    }
    contains(QT_CONFIG, phonon) {
        QT += phonon
    }
    contains(QT_CONFIG, xmlpatterns) {
        QT += xmlpatterns
    }
    contains(QT_CONFIG, webkit) {
        QT += webkit 
    }
}
symbian {
    TARGET.UID3 = 0x20021317
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    TARGET.CAPABILITY = NetworkServices ReadUserData
}
mac {
    QMAKE_INFO_PLIST=Info_mac.plist
    TARGET=QMLViewer
    ICON=qml.icns
} else {
    TARGET=qmlviewer
}
