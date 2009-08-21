load(qttest_p4)
SOURCES  += ../tst_qudpsocket.cpp
QT = core network

MOC_DIR=tmp
DEFINES += TEST_QNETWORK_PROXY

win32 {
  CONFIG(debug, debug|release) {
    DESTDIR = ../debug
} else {
    DESTDIR = ../release
  }
} else {
    DESTDIR = ../
}

wince*|symbian*: {
    addApp.sources = ../clientserver/clientserver.exe
    addApp.path = clientserver
    DEPLOYMENT += addApp
}

TARGET = tst_qudpsocket


