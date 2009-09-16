TEMPLATE = app
TARGET = weatherinfo
SOURCES = weatherinfo.cpp
RESOURCES = weatherinfo.qrc
QT += network svg

symbian {
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    TARGET.UID3 = 0xA000CF77
    HEADERS += $$QT_SOURCE_TREE/examples/network/ftp/sym_iap_util.h
    LIBS += -lesock -lconnmon -linsock
    TARGET.CAPABILITY = NetworkServices
}
