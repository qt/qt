load(qttest_p4)
TARGET = tst_qtextcodec
QT -= gui
SOURCES += main.cpp

wince*:{
   DEFINES += SRCDIR=\\\"\\\"
} else:symbian* {
   addFiles.sources = utf-8.txt
   addFiles.path = .
   DEPLOYMENT += addFiles
   TARGET.EPOCHEAPSIZE="0x100 0x1000000"
} else {
   DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

