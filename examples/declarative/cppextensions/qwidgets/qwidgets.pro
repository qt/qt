TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = QWidgets
TARGET = qwidgetsplugin

SOURCES += qwidgets.cpp

sources.files += qwidgets.pro qwidgets.cpp qwidgets.qml
sources.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins
target.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins

INSTALLS += sources target

symbian {
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1
}
