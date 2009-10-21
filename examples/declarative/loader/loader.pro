SOURCES = main.cpp
RESOURCES = loader.qrc

QT += script declarative network

target.path = $$[QT_INSTALL_EXAMPLES]/declarative/loader
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS loader.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/loader
INSTALLS += target sources

symbian {
#    TARGET.UID3 =
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCHEAPSIZE = 100000 20000000
    HEADERS += $$QT_SOURCE_TREE/examples/network/qftp/sym_iap_util.h
    LIBS += -lesock  -lconnmon -linsock
    TARGET.CAPABILITY = NetworkServices
}
