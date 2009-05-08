TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../..
HEADERS       = spin_ai.h
SOURCES       = spin_ai.cpp
TARGET        = $$qtLibraryTarget(spin_ai)
DESTDIR       = ../../errorstate/plugins

#! [0]
# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine/errorstate/plugins
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS spin_ai.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine/errorstateplugins/spin_ai