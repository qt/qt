include(../symbian.pri)

symbian {
    exists($${EPOCROOT}epoc32/release/winscw/udeb/cmmanager.lib)| \
    exists($${EPOCROOT}epoc32/release/armv5/lib/cmmanager.lib) {
        DEFINES += SNAP_FUNCTIONALITY_AVAILABLE
        LIBS += -lcmmanager
    } else {
        # Fall back to 3_1 implementation on platforms that do not have cmmanager
        LIBS += -lapengine
    }
}

TARGET = $${TARGET}_3_2
TARGET.UID3 = 0x2002131D
