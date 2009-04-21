DESTDIR = ../../bin
QT -= gui
# Input
SOURCES += qmlconv.cpp

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

CONFIG += console

