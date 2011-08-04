HEADERS   = downloader.h \
            ftpreply.h \
            ftpview.h \
            networkaccessmanager.h
SOURCES   = downloader.cpp \
            ftpreply.cpp \
            ftpview.cpp \
            main.cpp \
            networkaccessmanager.cpp

QT += network webkit

# install
target.path = $$[QT_INSTALL_EXAMPLES]/webkit/webftpclient
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/webkit/webftpclient
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000EFEF
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
