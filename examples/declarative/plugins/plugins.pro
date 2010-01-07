TEMPLATE = lib
TARGET  = qtimeexampleqmlplugin
CONFIG += qt plugin declarative

SOURCES += plugin.cpp

target.path += $$[QT_INSTALL_PLUGINS]/qmlmodules
INSTALLS += target


VERSION=1.0.0

