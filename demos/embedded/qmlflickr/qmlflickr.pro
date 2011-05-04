!symbian:!wince*:warning("DEPLOYMENT support required. This project only works on Symbian and WinCE.")

QT += declarative network
SOURCES += $$PWD/qmlflickr.cpp
include($$PWD/deployment.pri)

symbian {
    TARGET.UID3 = 0x$$qmlflickr_uid3 # defined in deployment.pri
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    TARGET.CAPABILITY = NetworkServices
    # Maximum heap size set to 128 MB in order to allow loading large images.
    TARGET.EPOCHEAPSIZE = 0x20000 0x8000000
}
