load(qttest_p4)
SOURCES += tst_qcssparser.cpp
DEFINES += SRCDIR=\\\"$$PWD\\\"
QT += xml


wince*: {
   addFiles.sources = testdata
   addFiles.path = .
   DEPLOYMENT += addFiles
}
