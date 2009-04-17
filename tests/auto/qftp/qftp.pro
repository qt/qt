load(qttest_p4)
SOURCES  += tst_qftp.cpp


QT = core network

wince*: {
   addFiles.sources = rfc3252.txt
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEFINES += SRCDIR=\\\"\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
