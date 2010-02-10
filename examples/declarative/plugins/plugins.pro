TEMPLATE = lib
TARGET  = qtimeexampleqmlplugin
CONFIG += qt plugin declarative

SOURCES += plugin.cpp

target.path += $$[QT_INSTALL_PLUGINS]/qmlmodules
sources.files += files/Clock.qml files/qmldir files/background.png files/center.png files/clock-night.png files/clock.png files/hour.png files/minute.png
sources.path += $$[QT_INSTALL_DATA]/qml/com/nokia/TimeExample
INSTALLS += target sources


VERSION=1.0.0

