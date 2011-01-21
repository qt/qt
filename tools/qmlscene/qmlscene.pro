TEMPLATE = app
TARGET = qmlscene
DESTDIR= ../../bin

QT += declarative

macx: CONFIG -= app_bundle

SOURCES += main.cpp

CONFIG += console

symbian {
    TARGET.EPOCHEAPSIZE = 0x20000 0x5000000
}

DEFINES += QML_RUNTIME_TESTING
