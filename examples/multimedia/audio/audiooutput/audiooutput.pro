HEADERS       = audiooutput.h
SOURCES       = audiooutput.cpp \
                main.cpp

QT           += multimedia

# install
target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audio/audiooutput
sources.files = $$SOURCES *.h $$RESOURCES $$FORMS audiooutput.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audio/audiooutput
INSTALLS += target sources
