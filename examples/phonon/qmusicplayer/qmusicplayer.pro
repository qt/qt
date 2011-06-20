QT        += phonon

HEADERS   += mainwindow.h
SOURCES   += main.cpp \
             mainwindow.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/phonon/qmusicplayer
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png images
sources.path = $$[QT_INSTALL_EXAMPLES]/phonon/qmusicplayer
INSTALLS += target sources

wince*{
    DEPLOYMENT_PLUGIN += phonon_ds9 phonon_waveout
}

symbian {
    TARGET.UID3 = 0xA000CF6A
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}

maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
symbian: warning(This example might not fully work on Symbian platform)
simulator: warning(This example might not fully work on Simulator platform)
