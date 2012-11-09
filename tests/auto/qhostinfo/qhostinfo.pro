CONFIG += testcase

TARGET = tst_qhostinfo
QT = core network testlib
SOURCES  += tst_qhostinfo.cpp

wince*: {
  LIBS += ws2.lib
} else {
  win32:LIBS += -lws2_32
}

symbian: TARGET.CAPABILITY = NetworkServices
symbian: {
  INCLUDEPATH *= $$MW_LAYER_SYSTEMINCLUDE
}

CONFIG+=insignificant_test # QTQAINFRA-574
