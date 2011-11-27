TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = QWidgets
TARGET = qmlqwidgetsplugin

SOURCES += qwidgets.cpp

sources.files += qwidgets.pro qwidgets.cpp qwidgets.qml
sources.path += $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/plugins
target.path += $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/plugins

INSTALLS += sources target

symbian:{
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1

    importFiles.files = QWidgets/qmlqwidgetsplugin.dll QWidgets/qmldir
    importFiles.path = QWidgets

    DEPLOYMENT += importFiles
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
