load(qttest_p4)
QT = core gui script
SOURCES += tst_qscriptengine.cpp 
DEFINES += SRCDIR=\\\"$$PWD\\\"

wince*|symbian*: {
   addFiles.sources = script
   addFiles.path = .
   DEPLOYMENT += addFiles
}

symbian: {
   TARGET.EPOCHEAPSIZE="0x100000 0x1000000 // Min 1Mb, max 16Mb"
}
