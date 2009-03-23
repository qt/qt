load(qttest_p4)
SOURCES += tst_qzip.cpp

wince*: {
   addFiles.sources = testdata
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEFINES += SRCDIR=\\\".\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}
