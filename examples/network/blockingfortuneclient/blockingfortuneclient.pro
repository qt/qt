HEADERS       = blockingclient.h \
                fortunethread.h
SOURCES       = blockingclient.cpp \
                main.cpp \
                fortunethread.cpp
QT           += network

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/blockingfortuneclient
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS blockingfortuneclient.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/network/blockingfortuneclient
INSTALLS += target sources

symbian: {
    TARGET.CAPABILITY = NetworkServices
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example might not fully work on Maemo platform)
