load(qttest_p4)

DEFINES += QLOCALSERVER_DEBUG
DEFINES += QLOCALSOCKET_DEBUG
!wince*: {
    DEFINES += SRCDIR=\\\"$$PWD/../\\\"
} else {
    DEFINES += QT_LOCALSOCKET_TCP
    DEFINES += SRCDIR=\\\"../\\\"
}

QT = core network

SOURCES += ../tst_qlocalsocket.cpp

TARGET = tst_qlocalsocket
CONFIG(debug_and_release) {
  CONFIG(debug, debug|release) {
    DESTDIR = ../debug
  } else {
    DESTDIR = ../release
  }
} else {
  DESTDIR = ..
}

wince* {
    additionalFiles.sources = ../lackey/lackey.exe
    additionalFiles.path = lackey
    scriptFiles.sources = ../lackey/scripts/*.js
    scriptFiles.path = lackey/scripts
    DEPLOYMENT = additionalFiles scriptFiles
    QT += script    # for easy deployment of QtScript
}

