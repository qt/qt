QT += phonon
TARGET = phonon_mmf
PHONON_MMF_DIR = $$QT_SOURCE_TREE/src/3rdparty/phonon/mmf
LIBS += -lDrmAudioPlayUtility.lib

HEADERS +=                                  \
           $$PHONON_MMF_DIR/audiooutput.h   \
           $$PHONON_MMF_DIR/backend.h       \
           $$PHONON_MMF_DIR/mediaobject.h

SOURCES +=                                  \
           $$PHONON_MMF_DIR/audiooutput.cpp \
           $$PHONON_MMF_DIR/backend.cpp     \
           $$PHONON_MMF_DIR/mediaobject.cpp

# This is needed for having the .qtplugin file properly created on Symbian.
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend

target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target

include(../../qpluginbase.pri)

# In the internal 5th SDK, DrmAudioSamplePlayer.h is placed in this folder, as
# opposed to the public, where it is placed in epoc32/include.
INCLUDEPATH *= /epoc32/include/osextensions

# We need this to be able to resolve ambiguity for VideoPlayer.h. Phonon and
# the SDK has the header.
INCLUDEPATH *= /epoc32

# Temporary steal one of the reserved, until we know that this MMF plugin is
# turning into something at all.
symbian:TARGET.UID3=0x2001E627
