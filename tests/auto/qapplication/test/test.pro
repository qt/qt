load(qttest_p4)

SOURCES += ../tst_qapplication.cpp
TARGET = ../tst_qapplication

wince* {
  additional.sources = ../desktopsettingsaware/desktopsettingsaware.exe
  additional.path = desktopsettingsaware
  someTest.sources = test.pro
  someTest.path = test
  DEPLOYMENT = additional deploy someTest
}

symbian: {
  additional.sources = ../desktopsettingsaware/desktopsettingsaware.exe
  additional.path = desktopsettingsaware
  someTest.sources = test.pro
  someTest.path = test
  windowIcon.sources = ../heart.svg
  DEPLOYMENT = additional deploy someTest windowIcon
  LIBS += -lcone -lavkon
}

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qapplication
} else {
    TARGET = ../../release/tst_qapplication
  }
}


