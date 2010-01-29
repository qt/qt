SOURCES  += tst_qnetworkconfigmanager.cpp
HEADERS  += ../qbearertestcommon.h
TARGET = tst_qnetworkconfigurationmanager
CONFIG += testcase

QT = core network

INCLUDEPATH += ../../../src/bearer

include(../../../common.pri)
CONFIG += mobility
MOBILITY = bearer

symbian {
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData
}

maemo6 {
    CONFIG += link_pkgconfig

    PKGCONFIG += conninet
}
