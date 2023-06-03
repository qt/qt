HEADERS = knob.h
SOURCES = main.cpp knob.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/touch/knobs
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS knobs.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/touch/knobs
INSTALLS += target sources

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
