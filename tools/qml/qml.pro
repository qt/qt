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
maemo5 {
    QT += maemo5
}
symbian {
    TARGET.UID3 = 0x20021317
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    TARGET.CAPABILITY = NetworkServices ReadUserData
    !contains(S60_VERSION, 3.1):!contains(S60_VERSION, 3.2) {
        LIBS += -lsensrvclient -lsensrvutil
    }
}
mac {
    QMAKE_INFO_PLIST=Info_mac.plist
    TARGET=QMLViewer
    ICON=qml.icns
} else {
    TARGET=qmlviewer
}
