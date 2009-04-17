load(qttest_p4)

SOURCES += tst_qsslkey.cpp
!wince*:win32:LIBS += -lws2_32
QT += network

TARGET = tst_qsslkey

win32 {
  CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
  }
}

wince*: {
   keyFiles.sources = keys
   keyFiles.path    = .
   DEPLOYMENT += keyFiles
   DEFINES += SRCDIR=\\\".\\\"
} else {
   DEFINES+= SRCDIR=\\\"$$PWD\\\"
}
