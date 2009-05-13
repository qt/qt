TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../..
HEADERS       = spin_ai.h
SOURCES       = spin_ai.cpp
TARGET        = $$qtLibraryTarget(spin_ai)
DESTDIR       = ../../tankgame/plugins

#! [0]
# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine/tankgame/plugins
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS spin_ai.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine/tankgameplugins/spin_ai