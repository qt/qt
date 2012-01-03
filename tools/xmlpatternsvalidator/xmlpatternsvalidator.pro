TEMPLATE = app
TARGET = xmlpatternsvalidator
DESTDIR = ../../bin
QT -= gui
QT += xmlpatterns

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

# This ensures we get stderr and stdout on Windows.
CONFIG += console

# This ensures that this is a command-line program on OS X and not a GUI application.
CONFIG -= app_bundle

SOURCES = main.cpp
HEADERS = main.h

symbian: TARGET.UID3 = 0xA000D7CA
