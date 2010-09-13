TEMPLATE = app
TARGET  = minehunt
QT += declarative
CONFIG += qt plugin

# Input
HEADERS += minehunt.h
SOURCES += main.cpp minehunt.cpp

sources.files = minehunt.qml minehunt.pro MinehuntCore
sources.path = $$[QT_INSTALL_DEMOS]/declarative/minehunt
target.path = $$[QT_INSTALL_DEMOS]/declarative/minehunt

INSTALLS = sources target

symbian:{
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    qmlminehuntfiles.sources = MinehuntCore minehunt.qml
    DEPLOYMENT = qmlminehuntfiles
}
 