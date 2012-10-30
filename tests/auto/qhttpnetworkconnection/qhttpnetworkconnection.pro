CONFIG += testcase

TARGET = tst_qhttpnetworkconnection
QT = core network testlib
SOURCES  += tst_qhttpnetworkconnection.cpp

INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/zlib
requires(contains(QT_CONFIG,private_tests))

symbian: TARGET.CAPABILITY = NetworkServices
symbian: {
  INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
}

win32:CONFIG+=insignificant_test # QTQAINFRA-574
