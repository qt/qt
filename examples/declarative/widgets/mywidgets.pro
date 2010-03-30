TEMPLATE = lib
DESTDIR = MyWidgets
TARGET  = mywidgetsplugin
CONFIG += qt plugin
QT += declarative
VERSION = 1.0.0

SOURCES += mywidgets.cpp

sources.files += mywidgets.pro mywidgets.cpp mywidgets.qml

sources.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins

target.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins

INSTALLS += sources target

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)

