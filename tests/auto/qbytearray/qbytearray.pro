load(qttest_p4)
SOURCES  += tst_qbytearray.cpp


QT = core

wince*|symbian|qnx {
   addFile.files = rfc3252.txt
   addFile.path = .
   DEPLOYMENT += addFile
}

wince*|qnx {
  DEFINES += SRCDIR=\\\"./\\\"
} else:symbian {
  TARGET.EPOCHEAPSIZE="0x100 0x800000"
} else {
  DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

CONFIG += parallel_test
