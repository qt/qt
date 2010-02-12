load(qttest_p4)
SOURCES  += tst_qnetworksession.cpp
HEADERS  += ../../qbearertestcommon.h

QT = core network

symbian {
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData PowerMgmt
}

maemo6 {
    CONFIG += link_pkgconfig

    PKGCONFIG += conninet
}
