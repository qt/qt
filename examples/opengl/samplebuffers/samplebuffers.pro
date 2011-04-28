HEADERS += glwidget.h
SOURCES += glwidget.cpp main.cpp

QT += opengl

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/samplebuffers
sources.files = $$SOURCES $$HEADERS samplebuffers.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/opengl/samplebuffers
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example does not work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
