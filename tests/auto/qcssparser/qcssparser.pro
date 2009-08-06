load(qttest_p4)
SOURCES += tst_cssparser.cpp
QT += xml

requires(contains(QT_CONFIG,private_tests))

!symbian: {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}

wince*|symbian: {
   addFiles.sources = testdata
   addFiles.path = .
   DEPLOYMENT += addFiles
}
