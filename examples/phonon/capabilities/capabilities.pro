QT      += phonon

HEADERS += window.h
SOURCES += window.cpp \
           main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/phonon/capabilities
sources.files = $$SOURCES $$HEADERS capabilities.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/phonon/capabilities
INSTALLS += target sources

wince*{
    DEPLOYMENT_PLUGIN += phonon_ds9 phonon_waveout
}

symbian {
    TARGET.UID3 = 0xA000CF69
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

simulator: warning(This example might not fully work on Simulator platform)
