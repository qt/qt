HEADERS       = audioinput.h
SOURCES       = audioinput.cpp \
                main.cpp

QT           += multimedia

# install
target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audio/audioinput
sources.files = $$SOURCES *.h $$RESOURCES $$FORMS audioinput.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audio/audioinput
INSTALLS += target sources

