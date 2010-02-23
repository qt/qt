TEMPLATE = lib
DESTDIR = com/nokia/TimeExample
TARGET  = qtimeexampleqmlplugin
CONFIG += qt plugin
QT += declarative

SOURCES += plugin.cpp

target.path += $$[QT_INSTALL_PLUGINS]/qmlmodules

sources.files += \
    $$PWD/com/nokia/TimeExample/qmldir \
    $$PWD/com/nokia/TimeExample/center.png \
    $$PWD/com/nokia/TimeExample/clock.png \
    $$PWD/com/nokia/TimeExample/Clock.qml \
    $$PWD/com/nokia/TimeExample/hour.png \
    $$PWD/com/nokia/TimeExample/minute.png

sources.path += $$[QT_INSTALL_DATA]/qml/com/nokia/TimeExample

INSTALLS += target sources

VERSION=1.0.0

