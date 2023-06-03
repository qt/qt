SOURCES += main.cpp
FORMS += dials.ui

# install
target.path = $$[QT_INSTALL_EXAMPLES]/touch/dials
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS dials.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/touch/dials
INSTALLS += target sources

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
