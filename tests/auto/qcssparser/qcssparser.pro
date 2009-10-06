load(qttest_p4)
SOURCES += tst_qcssparser.cpp
DEFINES += SRCDIR=\\\"$$PWD\\\"
QT += xml

wince* {
   addFiles.sources = testdata
   addFiles.path = .
   timesFont.sources = C:/Windows/Fonts/times.ttf
   timesFont.path = .
   DEPLOYMENT += addFiles timesFont
}

