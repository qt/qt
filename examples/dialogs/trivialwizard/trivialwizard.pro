SOURCES       = trivialwizard.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/dialogs/trivialwizard
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/dialogs/trivialwizard
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)
