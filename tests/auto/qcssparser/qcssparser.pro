load(qttest_p4)
SOURCES += tst_cssparser.cpp
DEFINES += SRCDIR=\\\"$$PWD\\\"
QT += xml


wince*: {
   addFiles.sources = testdata
   addFiles.path = .
   DEPLOYMENT += addFiles
}
