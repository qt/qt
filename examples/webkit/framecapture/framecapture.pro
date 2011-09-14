QT      += webkit network

HEADERS = framecapture.h
SOURCES = main.cpp \
          framecapture.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/framecapture
sources.files = $$SOURCES $$HEADERS
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/framecapture
INSTALLS += target sources

symbian {
    TARGET.CAPABILITY = NetworkServices
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example does not work on Symbian platform)
simulator: warning(This example does not work on Simulator platform)
