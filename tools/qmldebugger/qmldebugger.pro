DESTDIR = ../../bin
QT += network declarative
contains(QT_CONFIG, opengles2)|contains(QT_CONFIG, opengles1): QT += opengl

# Input
HEADERS += canvasframerate.h canvasscene.h
SOURCES += main.cpp canvasframerate.cpp canvasscene.cpp

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG += console
