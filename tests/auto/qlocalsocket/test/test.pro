load(qttest_p4)

include(../src/src.pri)

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
TARGET = ../tst_qlocalsocket

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qlocalsocket
} else {
    TARGET = ../../release/tst_qlocalsocket
  }
}

wince* {
    additionalFiles.sources = ../lackey/lackey.exe
    additionalFiles.path = lackey
    scriptFiles.sources = ../lackey/scripts/*.js
    scriptFiles.path = lackey/scripts
    DEPLOYMENT = additionalFiles scriptFiles
    QT += script    # for easy deployment of QtScript
}

