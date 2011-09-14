HEADERS       = audiooutput.h
SOURCES       = audiooutput.cpp \
                main.cpp

QT           += multimedia

# install
target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audiooutput
sources.files = $$SOURCES *.h $$RESOURCES $$FORMS audiooutput.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audiooutput
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000D7C0
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
maemo5: warning(This example might not fully work on Maemo platform)
