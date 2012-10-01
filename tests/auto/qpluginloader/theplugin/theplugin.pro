TEMPLATE      = lib
CONFIG       += plugin
HEADERS       = theplugin.h
SOURCES       = theplugin.cpp
# Use a predictable name for the plugin, no debug extension. Just like most apps do.
#TARGET        = $$qtLibraryTarget(theplugin)
TARGET        = theplugin

DESTDIR       = ../bin

symbian: {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY=ALL -TCB
}
