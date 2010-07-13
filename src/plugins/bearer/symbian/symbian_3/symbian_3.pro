include(../symbian.pri)

symbian {
    exists($${EPOCROOT}epoc32/release/winscw/udeb/cmmanager.lib)| \
    exists($${EPOCROOT}epoc32/release/armv5/lib/cmmanager.lib) {
        DEFINES += SNAP_FUNCTIONALITY_AVAILABLE
        LIBS += -lcmmanager

        exists($$MW_LAYER_PUBLIC_EXPORT_PATH(extendedconnpref.h)) {
            DEFINES += OCC_FUNCTIONALITY_AVAILABLE
            LIBS += -lextendedconnpref
        }
    } else {
        # Fall back to 3_1 implementation on platforms that do not have cmmanager
        LIBS += -lapengine
    }
}

TARGET.UID3 = 0x20021319
