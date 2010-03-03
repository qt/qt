TEMPLATE = lib
TARGET  = imageprovider
QT += declarative
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
DESTDIR = ImageProviderCore

# Input
SOURCES += imageprovider.cpp

