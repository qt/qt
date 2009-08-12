# MMF Phonon backend

QT += phonon
TARGET = phonon_mmf
PHONON_MMF_DIR = $$QT_SOURCE_TREE/src/3rdparty/phonon/mmf

# Uncomment the following line in order to use the CDrmPlayerUtility client
# API for audio playback, rather than CMdaAudioPlayerUtility.
#CONFIG += phonon_mmf_audio_drm

phonon_mmf_audio_drm {
    LIBS += -lDrmAudioPlayUtility.lib

    # In the internal 5th SDK, DrmAudioSamplePlayer.h is placed in this
    # folder, as opposed to the public, where it is placed in
    # epoc32/include.
    INCLUDEPATH *= /epoc32/include/osextensions

    DEFINES += QT_PHONON_MMF_AUDIO_DRM
} else {
    LIBS += -lmediaclientaudio.lib
}

HEADERS +=                                  \
           $$PHONON_MMF_DIR/audiooutput.h   \
           $$PHONON_MMF_DIR/backend.h       \
           $$PHONON_MMF_DIR/mediaobject.h   \
       $$PHONON_MMF_DIR/utils.h

SOURCES +=                                  \
           $$PHONON_MMF_DIR/audiooutput.cpp \
           $$PHONON_MMF_DIR/backend.cpp     \
           $$PHONON_MMF_DIR/mediaobject.cpp \
       $$PHONON_MMF_DIR/utils.cpp

# This is needed for having the .qtplugin file properly created on Symbian.
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend

target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target

include(../../qpluginbase.pri)

# We need this to be able to resolve ambiguity for VideoPlayer.h. Phonon and
# the SDK has the header.
INCLUDEPATH *= /epoc32

# Temporary steal one of the reserved, until we know that this MMF plugin is
# turning into something at all.
symbian:TARGET.UID3=0x2001E627
