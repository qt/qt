SOURCES  += tst_qnetworksession.cpp
HEADERS  += ../../qbearertestcommon.h
TARGET = tst_qnetworksession
CONFIG += testcase

QT = core network

INCLUDEPATH += ../../../../src/bearer

include(../../../../common.pri)
qtAddLibrary(QtBearer)

wince* {
    LACKEY.sources = $$OUTPUT_DIR/build/tests/bin/qnetworksessionlackey.exe
    LACKEY.path = .
    DEPLOYMENT += LACKEY
}

symbian {
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData
}
