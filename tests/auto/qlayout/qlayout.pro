CONFIG += testcase

TARGET = tst_qlayout
QT += testlib
SOURCES += tst_qlayout.cpp

contains(QT_CONFIG, qt3support): QT += qt3support
wince*|symbian: {
   wince*:DEFINES += SRCDIR=\\\"\\\"
   addFiles.files = baseline
   addFiles.path = .
   DEPLOYMENT += addFiles
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"

   test_data.files = baseline/*
   test_data.path =  $${target.path}/baseline
   INSTALLS += test_data
}

