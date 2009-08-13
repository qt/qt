HEADERS       = audiodevices.h
SOURCES       = audiodevices.cpp \
                main.cpp
FORMS         += audiodevicesbase.ui

QT           += multimedia

# install
target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audio/audiodevices
sources.files = $$SOURCES *.h $$RESOURCES $$FORMS audiodevices.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audio/audiodevices
INSTALLS += target sources
