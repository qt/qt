load(qttest_p4)

SOURCES += ../tst_q3process.cpp
TARGET = ../tst_q3process

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_q3process
} else {
    TARGET = ../../release/tst_q3process
  }
}

contains(QT_CONFIG, qt3support): QT += qt3support


