TEMPLATE = app
TARGET = flightinfo
SOURCES = flightinfo.cpp
FORMS += form.ui
RESOURCES = flightinfo.qrc
QT += network

symbian {
    HEADERS += $$QT_SOURCE_TREE/examples/network/ftp/sym_iap_util.h
    LIBS += -lesock  -lconnmon
    TARGET.CAPABILITY = NetworkServices
}
