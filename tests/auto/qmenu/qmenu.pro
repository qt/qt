load(qttest_p4)
SOURCES  += tst_qmenu.cpp

macx {
    LIBS += -framework AppKit
    OBJECTIVE_SOURCES += tst_qmenu_mac_helpers.mm
    HEADERS += tst_qmenu_mac_helpers.h
}

contains(QT_CONFIG, qt3support): QT += qt3support

