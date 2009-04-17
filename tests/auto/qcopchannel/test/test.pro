load(qttest_p4)

SOURCES += ../tst_qcopchannel.cpp
TARGET = ../tst_qcopchannel

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qcopchannel
} else {
    TARGET = ../../release/tst_qcopchannel
  }
}


