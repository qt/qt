load(qttest_p4)
HEADERS += 
SOURCES += tst_qtextboundaryfinder.cpp 
DEFINES += SRCDIR=\\\"$$PWD\\\"

wince*:{
   addFiles.sources = data
   addFiles.path = .
   DEPLOYMENT += addFiles
}
