CONFIG += testcase
TARGET = tst_qnetworkconfigurationmanager
SOURCES  += tst_qnetworkconfigurationmanager.cpp
HEADERS  += ../qbearertestcommon.h

QT = core network testlib

symbian {
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData
}

maemo6|maemo5 {
    CONFIG += link_pkgconfig

    PKGCONFIG += conninet
}
