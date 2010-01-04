SOURCES  += tst_qnetworkconfiguration.cpp
HEADERS  += ../qbearertestcommon.h
TARGET = tst_qnetworkconfiguration
CONFIG += testcase

QT = core network

INCLUDEPATH += ../../../src/bearer

include(../../../common.pri)
CONFIG += mobility
MOBILITY = bearer

symbian {
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData 
}
