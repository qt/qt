DESTDIR = ../../bin
QT += network declarative
# Input
HEADERS += canvasframerate.h
SOURCES += main.cpp canvasframerate.cpp

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG += console
