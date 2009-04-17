load(qttest_p4)
SOURCES  += ../tst_qnetworkreply.cpp
TARGET = ../tst_qnetworkreply

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qnetworkreply
} else {
    TARGET = ../../release/tst_qnetworkreply
  }
}

DEFINES += SRCDIR=\\\"$$PWD/..\\\"

QT = core network
RESOURCES += ../qnetworkreply.qrc

wince*: {
    addFiles.sources = ../empty ../rfc3252.txt ../resource
    addFiles.path = .
    DEPLOYMENT += addFiles
}
