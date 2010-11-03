TEMPLATE = lib
CONFIG += plugin
SOURCES = simpleplugin.cpp
QT = core script
TARGET = simpleplugin
DESTDIR = ../plugins/script

symbian {
    TARGET.EPOCALLOWDLLDATA=1
}
