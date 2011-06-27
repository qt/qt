#! [0]
CONFIG      += uitools
SOURCES     = main.cpp
RESOURCES   = worldtimeclockbuilder.qrc
#! [0]

# install
target.path = $$[QT_INSTALL_EXAMPLES]/designer/worldtimeclockbuilder
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.ui *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/designer/worldtimeclockbuilder
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
symbian: warning(This example does not work on Symbian platform)
maemo5: warning(This example does not work on Maemo platform)
simulator: warning(This example does not work on Simulator platform)
