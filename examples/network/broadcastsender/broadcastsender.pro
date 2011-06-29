HEADERS       = sender.h
SOURCES       = sender.cpp \
                main.cpp
QT           += network

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/broadcastsender
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS broadcastsender.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/network/broadcastsender
INSTALLS += target sources

symbian: {
    TARGET.CAPABILITY = NetworkServices
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

