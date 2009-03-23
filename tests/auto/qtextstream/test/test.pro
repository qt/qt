load(qttest_p4)
SOURCES  += ../tst_qtextstream.cpp

TARGET = ../tst_qtextstream

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qtextstream
} else {
    TARGET = ../../release/tst_qtextstream
  }
}

RESOURCES += ../qtextstream.qrc

contains(QT_CONFIG, qt3support):QT += qt3support
QT = core network


wince*: {
   addFiles.sources = ../rfc3261.txt ../shift-jis.txt ../task113817.txt ../qtextstream.qrc ../tst_qtextstream.cpp
   addFiles.path = .
   res.sources = ../resources
   res.path = .
   DEPLOYMENT += addFiles
   DEFINES += SRCDIR=\\\"\\\"
} else {
  DEFINES += SRCDIR=\\\"$$PWD/../\\\"
}


