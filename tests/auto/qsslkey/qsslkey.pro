load(qttest_p4)

SOURCES += tst_qsslkey.cpp
!wince*:win32:LIBS += -lws2_32
QT = core network

TARGET = tst_qsslkey

win32 {
  CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
  }
}

wince*|symbian: {
   keyFiles.sources = keys
   keyFiles.path    = .
   DEPLOYMENT += keyFiles
}

wince*: {
   DEFINES += SRCDIR=\\\".\\\"
} else:!symbian {
   DEFINES+= SRCDIR=\\\"$$PWD\\\"
}
