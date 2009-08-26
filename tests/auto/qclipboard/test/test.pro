load(qttest_p4)
SOURCES  += ../tst_qclipboard.cpp
TARGET = ../tst_qclipboard

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qclipboard
} else {
    TARGET = ../../release/tst_qclipboard
  }
}

wince*|symbian*: {
  copier.sources = ../copier/copier.exe
  copier.path = copier
  paster.sources = ../paster/paster.exe
  paster.path = paster
  DEPLOYMENT = copier paster
}