HEADERS       = server.h
SOURCES       = server.cpp \
                main.cpp
QT           += network

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/fortuneserver
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS fortuneserver.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/network/fortuneserver
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

symbian {
    HEADERS += $$QT_SOURCE_TREE/examples/network/ftp/sym_iap_util.h
    LIBS += -lesock
    TARGET.UID3 = 0xA000CF71
    TARGET.CAPABILITY = "All -TCB"
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}