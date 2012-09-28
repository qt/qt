CONFIG += testcase
TARGET = ../tst_selftests
QT = core xml testlib
SOURCES += ../tst_selftests.cpp

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_selftests
  } else {
    TARGET = ../../release/tst_selftests
  }
}

RESOURCES += ../selftests.qrc
