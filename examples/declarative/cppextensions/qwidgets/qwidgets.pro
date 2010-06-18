TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = QWidgets
TARGET = qmlqwidgetsplugin

SOURCES += qwidgets.cpp

sources.files += qwidgets.pro qwidgets.cpp qwidgets.qml
sources.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins
target.path += $$[QT_INSTALL_EXAMPLES]/declarative/plugins

INSTALLS += sources target

symbian:{
    load(data_caging_paths)
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1

    importFiles.sources = qmlqwidgetsplugin.dll QWidgets/qmldir
    importFiles.path = QWidgets

    DEPLOYMENT = importFiles
}
