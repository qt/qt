load(qttest_p4)
SOURCES += tst_cssparser.cpp
QT += xml

!symbian: {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}

wince*|symbian: {
   addFiles.sources = testdata
   addFiles.path = .
   DEPLOYMENT += addFiles
}
