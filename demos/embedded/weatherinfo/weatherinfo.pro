TEMPLATE = app
TARGET = weatherinfo
SOURCES = weatherinfo.cpp
RESOURCES = weatherinfo.qrc
QT += network svg

symbian {
    TARGET.UID3 = 0xA000CF77
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    HEADERS += $$QT_SOURCE_TREE/examples/network/qftp/sym_iap_util.h
    LIBS += -lesock -lconnmon -linsock
    TARGET.CAPABILITY = NetworkServices
}
