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

