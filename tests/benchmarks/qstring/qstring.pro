load(qttest_p4)
TARGET = tst_qstring
QT -= gui
SOURCES += main.cpp

wince*:{
   DEFINES += SRCDIR=\\\"\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD/\\\"
}


