TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../..
HEADERS       = random_ai_plugin.h
SOURCES       = random_ai_plugin.cpp
TARGET        = $$qtLibraryTarget(random_ai)
DESTDIR       = ../../errorstate/plugins

#! [0]
# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine/errorstate/plugins
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS random_ai.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine/errorstateplugins/random_ai