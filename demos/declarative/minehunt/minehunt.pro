TEMPLATE = lib
TARGET  = minehunt
QT += declarative
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
DESTDIR = MinehuntCore

# Input
SOURCES += minehunt.cpp


sources.files = minehunt.qml minehunt.pro
sources.path = $$[QT_INSTALL_DEMOS]/declarative/minehunt

target.path = $$[QT_INSTALL_DEMOS]/declarative/minehunt/MinehuntCore

MinehuntCore_sources.files = \
    MinehuntCore/Explosion.qml \
    MinehuntCore/pics \
    MinehuntCore/qmldir
MinehuntCore_sources.path = $$[QT_INSTALL_DEMOS]/declarative/minehunt/MinehuntCore

INSTALLS = sources MinehuntCore_sources target

symbian:{
    load(data_caging_paths)
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    
    importFiles.sources = minehunt.dll \
    MinehuntCore/Explosion.qml \
    MinehuntCore/pics \
    MinehuntCore/qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/MinehuntCore 
    DEPLOYMENT = importFiles
}
 
INSTALLS = sources MinehuntCore_sources target
