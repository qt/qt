load(qttest_p4)

SOURCES += ../tst_qdirectpainter.cpp
TARGET = ../tst_qdirectpainter

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qdirectpainter
} else {
    TARGET = ../../release/tst_qdirectpainter
  }
}


