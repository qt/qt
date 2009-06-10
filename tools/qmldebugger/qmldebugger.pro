DESTDIR = ../../bin
QT += network declarative
# Input
HEADERS += canvasframerate.h canvasscene.h
SOURCES += main.cpp canvasframerate.cpp canvasscene.cpp

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG += console
