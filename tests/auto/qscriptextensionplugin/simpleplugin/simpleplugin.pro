TEMPLATE = lib
CONFIG += plugin
SOURCES = simpleplugin.cpp
QT = core script
TARGET = simpleplugin
DESTDIR = ../plugins/script

symbian {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET = qscriptextension_simpleplugin  # Too generic target causes conflict on Symbian
}
