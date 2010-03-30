TEMPLATE = lib
TARGET  = imageprovider
QT += declarative
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
DESTDIR = ImageProviderCore

# Input
SOURCES += imageprovider.cpp

sources.files = $$SOURCES imageprovider.qml imageprovider.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/imageprovider

target.path = $$[QT_INSTALL_EXAMPLES]/declarative/imageprovider/ImageProviderCore

ImageProviderCore_sources.files = \
    ImageProviderCore/qmldir 
ImageProviderCore_sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/imageprovider/ImageProviderCore

INSTALLS = sources ImageProviderCore_sources target
