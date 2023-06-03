HEADERS       = ../connection.h
SOURCES       = tablemodel.cpp
QT           += sql

# install
target.path = $$[QT_INSTALL_EXAMPLES]/sql/tablemodel
sources.files = $$SOURCES *.h $$RESOURCES $$FORMS tablemodel.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/sql/tablemodel
INSTALLS += target sources

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

