TEMPLATE = app
SOURCES = lightmaps.cpp
QT += network

symbian {
    TARGET.UID3 = 0xA000CF75
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    HEADERS += $$QT_SOURCE_TREE/examples/network/qftp/sym_iap_util.h
    LIBS += -lesock -lcommdb -linsock # For IAP selection
    TARGET.CAPABILITY = NetworkServices
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

target.path = $$[QT_INSTALL_DEMOS]/embedded/lightmaps
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro
sources.path = $$[QT_INSTALL_DEMOS]/embedded/lightmaps
INSTALLS += target sources
