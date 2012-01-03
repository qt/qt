CONFIG += testcase

TARGET = tst_qsslcertificate
QT = core network testlib
SOURCES += tst_qsslcertificate.cpp
!wince*:win32:LIBS += -lws2_32

win32 {
  CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
  }
}

wince*|symbian: {
  certFiles.files = certificates more-certificates
  certFiles.path    = .
  DEPLOYMENT += certFiles
}

wince*: {
  DEFINES += SRCDIR=\\\".\\\"
} else:!symbian {
   DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

symbian:TARGET.CAPABILITY = NetworkServices ReadUserData
