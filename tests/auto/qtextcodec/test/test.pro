load(qttest_p4)
TARGET = ../tst_qtextcodec
SOURCES  += ../tst_qtextcodec.cpp
wince*|symbian {
   addFiles.sources = ../*.txt
   addFiles.path = .
   DEPLOYMENT += addFiles
}



