load(qttest_p4)

SOURCES += tst_qsslsocket.cpp
!wince*:win32:LIBS += -lws2_32
QT += network
QT -= gui

TARGET = tst_qsslsocket

!wince* {
DEFINES += SRCDIR=\\\"$$PWD/\\\"
} else {
DEFINES += SRCDIR=\\\"./\\\"
}

win32 {
  CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
  }
}

wince*: {
   certFiles.sources = certs ssl.tar.gz
   certFiles.path    = .
   DEPLOYMENT += certFiles
}
