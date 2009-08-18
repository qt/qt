load(qttest_p4)

SOURCES  += ../tst_qtextcodec.cpp

!wince*: {
TARGET = ../tst_qtextcodec

win32: {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qtextcodec
} else {
    TARGET = ../../release/tst_qtextcodec
  }
}
} else {
   TARGET = tst_qtextcodec
}

wince*: {
   addFiles.sources = ../*.txt
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEPLOYMENT_PLUGIN += qcncodecs qjpcodecs qkrcodecs qtwcodecs
   DEFINES += SRCDIR=\\\"\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD/../\\\"
}
