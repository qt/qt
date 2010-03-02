TEMPLATE = lib
TARGET  = minehunt
QT += declarative
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
DESTDIR = MinehuntCore

# Input
SOURCES += minehunt.cpp

