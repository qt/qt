SOURCES = main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/statemachine/twowaybutton
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS twowaybutton.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/statemachine/twowaybutton
INSTALLS += target sources

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

