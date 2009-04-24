load(qttest_p4)

SOURCES += tst_qsslsocket.cpp
!wince*:win32:LIBS += -lws2_32
QT += network
QT -= gui

TARGET = tst_qsslsocket

win32 {
  CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
  }
}


symbian:{
    DEFINES += QSSLSOCKET_CERTUNTRUSTED_WORKAROUND
    TARGET.EPOCHEAPSIZE="0x100 0x1000000"
    TARGET.CAPABILITY="ALL -TCB"
}
wince*|symbian: {
   certFiles.sources = certs ssl.tar.gz
   certFiles.path    = .
   DEPLOYMENT += certFiles
}
