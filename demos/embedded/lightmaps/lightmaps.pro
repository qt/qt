TEMPLATE = app
SOURCES = lightmaps.cpp
QT += network

symbian {
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    TARGET.UID3 = 0xA000CF75
    HEADERS += $$QT_SOURCE_TREE/examples/network/ftp/sym_iap_util.h
    LIBS += -lesock  -lconnmon -linsock
    TARGET.CAPABILITY = NetworkServices
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
