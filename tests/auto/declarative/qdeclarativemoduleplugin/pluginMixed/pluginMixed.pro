TEMPLATE = lib
CONFIG += plugin
SOURCES = plugin.cpp
QT = core declarative
DESTDIR = ../imports/com/nokia/AutoTestQmlMixedPluginType

symbian: {
    TARGET.EPOCALLOWDLLDATA=1
}
