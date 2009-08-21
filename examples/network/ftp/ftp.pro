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

symbian {
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    HEADERS +=  sym_iap_util.h
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    TARGET.CAPABILITY="NetworkServices ReadUserData WriteUserData"
    TARGET.UID3 = 0xA000A648
    LIBS+=-lesock -lcommdb -linsock # For IAP selection
}
