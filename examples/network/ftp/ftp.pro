HEADERS       = ftpwindow.h
SOURCES       = ftpwindow.cpp \
                main.cpp
RESOURCES    += ftp.qrc
QT           += network

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/ftp
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS ftp.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/network/ftp
INSTALLS += target sources

include($$QT_SOURCE_TREE/examples/examplebase.pri)

symbian {
    HEADERS +=  sym_iap_util.h
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    TARGET.CAPABILITY="NetworkServices"
    TARGET.UID3 = 0xA000A648
    LIBS+=-lesock   # For IAP selection
}