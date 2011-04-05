TEMPLATE = app
TARGET = painteditem

QT += declarative

macx: CONFIG -= app_bundle

SOURCES += main.cpp

CONFIG += console

symbian {
    TARGET.EPOCHEAPSIZE = 0x20000 0x5000000
}
