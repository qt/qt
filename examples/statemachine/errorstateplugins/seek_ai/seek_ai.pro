TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../..
HEADERS       = seek_ai.h
SOURCES       = seek_ai.cpp
TARGET        = $$qtLibraryTarget(seek_ai)
DESTDIR       = ../../errorstate/plugins

#! [0]
# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine/errorstate/plugins
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS seek_ai.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine/errorstateplugins/seek_ai