TEMPLATE = lib
CONFIG += plugin
SOURCES = plugin.cpp
QT = core declarative
DESTDIR = ../imports/com/nokia/AutoTestQmlPluginType.2.1

symbian: {
    TARGET.EPOCALLOWDLLDATA=1
}
