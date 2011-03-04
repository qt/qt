load(qttest_p4)
QT -= gui
SOURCES  += ../tst_qnetworkreply.cpp
TARGET = ../tst_qnetworkreply

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qnetworkreply
} else {
    TARGET = ../../release/tst_qnetworkreply
  }
}

!symbian:DEFINES += SRCDIR=\\\"$$PWD/..\\\"

QT = core network
RESOURCES += ../qnetworkreply.qrc

wince*: {
    addFiles.files = ../empty ../rfc3252.txt ../resource
    addFiles.path = .
    DEPLOYMENT += addFiles
}

symbian:{
    addFiles.files = ../empty ../rfc3252.txt ../resource ../bigfile
    addFiles.path = .
    DEPLOYMENT += addFiles

    certFiles.files = ../certs
    certFiles.path    = .
    DEPLOYMENT += certFiles

    # Symbian toolchain does not support correct include semantics
    INCLUDEPATH+=..\\..\\..\\..\\include\\QtNetwork\\private
    # bigfile test case requires more heap
    TARGET.EPOCHEAPSIZE="0x100 0x10000000"
    TARGET.CAPABILITY="ALL -TCB"
}
