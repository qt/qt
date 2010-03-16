TEMPLATE = app
TARGET = flightinfo
SOURCES = flightinfo.cpp
FORMS += form.ui
RESOURCES = flightinfo.qrc
QT += network

symbian {
    TARGET.UID3 = 0xA000CF74
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    INCLUDEPATH += $$QT_SOURCE_TREE/examples/network/qftp/
    LIBS += -lesock -lcommdb -linsock # For IAP selection
    TARGET.CAPABILITY = NetworkServices
}

target.path = $$[QT_INSTALL_DEMOS]/embedded/flightinfo
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro
sources.path = $$[QT_INSTALL_DEMOS]/embedded/flightinfo
INSTALLS += target sources
