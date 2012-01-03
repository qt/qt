CONFIG += testcase

TARGET = tst_qzip
QT += testlib
SOURCES += tst_qzip.cpp

wince*|symbian: {
   addFiles.files = testdata
   addFiles.path = .
   DEPLOYMENT += addFiles
   !symbian:DEFINES += SRCDIR=\\\".\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}
