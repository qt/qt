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

wince*|symbian {
   addFiles.sources = ../*.txt
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEPLOYMENT_PLUGIN += qcncodecs qjpcodecs qkrcodecs qtwcodecs
}

wince*: {
   DEFINES += SRCDIR=\\\"\\\"
}else:symbian {
    # Symbian can't define SRCDIR meaningfully here
} else {
   DEFINES += SRCDIR=\\\"$$PWD/../\\\"
}
