TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../..
HEADERS       = spin_ai_with_error.h
SOURCES       = spin_ai_with_error.cpp
TARGET        = $$qtLibraryTarget(spin_ai_with_error)
DESTDIR       = ../../tankgame/plugins

#! [0]
# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine/tankgame/plugins
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS spin_ai_with_error.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine/tankgameplugins/spin_ai_with_error