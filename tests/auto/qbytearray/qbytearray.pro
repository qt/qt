load(qttest_p4)
SOURCES  += tst_qbytearray.cpp


QT = core

wince*: {
   addFile.sources = rfc3252.txt
   addFile.path = .
   DEPLOYMENT += addFile
   DEFINES += SRCDIR=\\\"\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
