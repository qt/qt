load(qttest_p4)
SOURCES += tst_qcssparser.cpp
QT += xml

requires(contains(QT_CONFIG,private_tests))
!symbian: {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}

wince*|symbian: {
   addFiles.files = testdata
   addFiles.path = .
   DEPLOYMENT += addFiles
}
wince* {
   timesFont.files = c:/windows/fonts/times.ttf
   timesFont.path = .
   DEPLOYMENT += timesFont
}

