!symbian:!wince*:warning("DEPLOYMENT support required. This project only works on Symbian and WinCE.")

QT += declarative network
SOURCES += $$PWD/qmltwitter.cpp
include($$PWD/deployment.pri)

symbian {
    TARGET.UID3 = 0x$$qmltwitter_uid3 # defined in deployment.pri
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    INCLUDEPATH += $$QT_SOURCE_TREE/examples/network/qftp/
    LIBS += -lesock -lcommdb -linsock # For IAP selection
    TARGET.CAPABILITY = NetworkServices
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
