DESTDIR = ../../bin
QT += network declarative
contains(QT_CONFIG, opengles2)|contains(QT_CONFIG, opengles1): QT += opengl

# Input
HEADERS += canvasframerate.h canvasscene.h engine.h
SOURCES += main.cpp canvasframerate.cpp canvasscene.cpp engine.cpp

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG += console
