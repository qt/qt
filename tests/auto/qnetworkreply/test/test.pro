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

!symbian:DEFINES += SRCDIR=\\\"$$PWD/..\\\"

QT = core network
RESOURCES += ../qnetworkreply.qrc

wince*: {
    addFiles.sources = ../empty ../rfc3252.txt ../resource
    addFiles.path = .
    DEPLOYMENT += addFiles
}

symbian:{
    addFiles.sources = ../empty ../rfc3252.txt ../resource ../bigfile
    addFiles.path = .
    DEPLOYMENT += addFiles

    certFiles.sources = ../certs
    certFiles.path    = .
    DEPLOYMENT += certFiles

    # Symbian toolchain does not support correct include semantics
    INCPATH+=..\\..\\..\\..\\include\\QtNetwork\\private
    # bigfile test case requires more heap
    TARGET.EPOCHEAPSIZE="0x100 0x1000000"
    TARGET.CAPABILITY="ALL -TCB"
}
