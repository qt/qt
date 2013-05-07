CONFIG += testcase

TARGET = tst_qnetworksession
QT = core network testlib
SOURCES  += tst_qnetworksession.cpp
HEADERS  += ../../qbearertestcommon.h

CONFIG(debug_and_release) {
  CONFIG(debug, debug|release) {
    DESTDIR = ../debug
  } else {
    DESTDIR = ../release
  }
} else {
  DESTDIR = ..
}

symbian {
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData PowerMgmt
}

maemo6|maemo5 {
    CONFIG += link_pkgconfig

    PKGCONFIG += conninet
}

win32:CONFIG+=insignificant_test # QTQAINFRA-574
