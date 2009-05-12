TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../..
HEADERS       = seek_ai.h
SOURCES       = seek_ai.cpp
TARGET        = $$qtLibraryTarget(seek_ai)
DESTDIR       = ../../tankgame/plugins

#! [0]
# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine/tankgame/plugins
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS seek_ai.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine/tankgameplugins/seek_ai