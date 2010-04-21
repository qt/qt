TEMPLATE = lib
DESTDIR = ProxyWidgets
TARGET  = proxywidgetsplugin
CONFIG += qt plugin
QT += declarative
VERSION = 1.0.0

SOURCES += proxywidgets.cpp

sources.files += proxywidgets.pro proxywidgets.cpp proxywidgets.qml

sources.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins

target.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins

INSTALLS += sources target

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)

symbian:{
    TARGET.EPOCALLOWDLLDATA = 1
}