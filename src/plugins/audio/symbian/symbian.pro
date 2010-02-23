QT += multimedia
TARGET = qaudio

# Paths to DevSound headers
INCLUDEPATH += /epoc32/include/mmf/common
INCLUDEPATH += /epoc32/include/mmf/server

HEADERS +=                              \
           symbianaudio.h               \
           symbianaudiodeviceinfo.h     \
           symbianaudioinput.h          \
           symbianaudiooutput.h         \
           symbianaudioutils.h

SOURCES +=                              \
           main.cpp                     \
           symbianaudiodeviceinfo.cpp   \
           symbianaudioinput.cpp        \
           symbianaudiooutput.cpp       \
           symbianaudioutils.cpp

LIBS += -lmmfdevsound

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/audio
target.path = $$[QT_INSTALL_PLUGINS]/audio
INSTALLS += target

include(../../qpluginbase.pri)

TARGET.UID3 = 0x2001E630

