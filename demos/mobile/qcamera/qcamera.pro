TEMPLATE = app
TARGET = qcamera

VERSION = 1.1.0

QT += network

HEADERS += contactsdlg.h \
    cameraexample.h \
    messagehandling.h \
    button.h \
    businesscardhandling.h

SOURCES += contactsdlg.cpp \
    main.cpp \
    cameraexample.cpp \
    messagehandling.cpp \
    button.cpp \
    businesscardhandling.cpp

CONFIG += mobility
MOBILITY = contacts \
    messaging \
    multimedia \
    systeminfo \
    location

RESOURCES += resources.qrc

ICON = icons/cameramms_icon.svg

symbian: {
    # Because landscape orientation lock
    LIBS += -lcone -leikcore -lavkon
    TARGET.UID3 = 0xEF642F0E
    TARGET = QCamera
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x20000 0x8000000

    # Self-signing capabilities
    TARGET.CAPABILITY += NetworkServices \
        ReadUserData \
        WriteUserData \
        LocalServices \
        UserEnvironment

    # QtMobility Messaging module needs these
    #DEFINES += MESSAGING_ENABLED

    contains(DEFINES,MESSAGING_ENABLED) {
        # Additional capabilities that needs Open Signed Online signing
        TARGET.CAPABILITY += ReadDeviceData WriteDeviceData
    }
}

!contains(DEFINES,MESSAGING_ENABLED) {
    warning(Qt Mobility Messaging disabled!)
}

