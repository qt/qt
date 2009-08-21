HEADERS       = client.h
SOURCES       = client.cpp \
                main.cpp
QT           += network

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/fortuneclient
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS fortuneclient.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/network/fortuneclient
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

symbian {
    HEADERS += $$QT_SOURCE_TREE/examples/network/ftp/sym_iap_util.h
    LIBS += -lesock
    TARGET.CAPABILITY = "NetworkServices ReadUserData WriteUserData"
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
