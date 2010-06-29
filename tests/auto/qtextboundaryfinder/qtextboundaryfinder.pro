load(qttest_p4)
QT = core
HEADERS += 
SOURCES += tst_qtextboundaryfinder.cpp 
!symbian:*:DEFINES += SRCDIR=\\\"$$PWD\\\"

wince*|symbian:{
   addFiles.sources = data
   addFiles.path = .
   DEPLOYMENT += addFiles
}
