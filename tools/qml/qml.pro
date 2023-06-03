TEMPLATE = app
CONFIG += qt uic declarative_debug
DESTDIR = ../../bin

include(qml.pri)

SOURCES += main.cpp

INCLUDEPATH += ../../include/QtDeclarative
INCLUDEPATH += ../../src/declarative/util
INCLUDEPATH += ../../src/declarative/graphicsitems

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

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

mac {
    QMAKE_INFO_PLIST=Info_mac.plist
    TARGET=QMLViewer
    ICON=qml.icns
} else {
    TARGET=qmlviewer
}
