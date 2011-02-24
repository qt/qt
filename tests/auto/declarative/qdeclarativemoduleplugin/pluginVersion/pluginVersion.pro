TEMPLATE = lib
CONFIG += plugin
SOURCES = plugin.cpp
QT = core declarative
DESTDIR = ../imports/com/nokia/AutoTestQmlVersionPluginType

symbian: {
    TARGET.EPOCALLOWDLLDATA=1
}
