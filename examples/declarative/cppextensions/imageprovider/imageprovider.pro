TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = ImageProviderCore
TARGET  = qmlimageproviderplugin

SOURCES += imageprovider.cpp

sources.files = $$SOURCES imageprovider.qml imageprovider.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/imageprovider

target.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/imageprovider/ImageProviderCore

ImageProviderCore_sources.files = \
    ImageProviderCore/qmldir 
ImageProviderCore_sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/imageprovider/ImageProviderCore

INSTALLS = sources ImageProviderCore_sources target

symbian:{
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1

    importFiles.files = ImageProviderCore/qmlimageproviderplugin.dll ImageProviderCore/qmldir
    importFiles.path = ImageProviderCore
    DEPLOYMENT += importFiles
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
