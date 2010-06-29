TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = ImageProviderCore
TARGET  = qmlimageproviderplugin

SOURCES += imageprovider.cpp

sources.files = $$SOURCES imageprovider.qml imageprovider.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/imageprovider

target.path = $$[QT_INSTALL_EXAMPLES]/declarative/imageprovider/ImageProviderCore

ImageProviderCore_sources.files = \
    ImageProviderCore/qmldir 
ImageProviderCore_sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/imageprovider/ImageProviderCore

INSTALLS = sources ImageProviderCore_sources target

symbian:{
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1

    importFiles.sources = ImageProviderCore/qmlimageproviderplugin.dll ImageProviderCore/qmldir
    importFiles.path = ImageProviderCore
    DEPLOYMENT = importFiles
}
