TEMPLATE = app

DESTDIR     = ../../bin

DEPENDPATH += .
INCLUDEPATH += . ../../src/3rdparty/harfbuzz/src
TARGET = qttracereplay

# Input
SOURCES += main.cpp

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG += console
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
