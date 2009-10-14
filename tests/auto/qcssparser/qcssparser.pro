load(qttest_p4)
SOURCES += tst_qcssparser.cpp
QT += xml

requires(contains(QT_CONFIG,private_tests))
!symbian: {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}

wince*|symbian: {
   addFiles.sources = testdata
   addFiles.path = .
   timesFont.sources = C:/Windows/Fonts/times.ttf
   timesFont.path = .
   DEPLOYMENT += addFiles timesFont
}

