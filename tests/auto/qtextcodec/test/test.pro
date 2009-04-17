load(qttest_p4)
TARGET = ../tst_qtextcodec
SOURCES  += ../tst_qtextcodec.cpp
wince*: {
   addFiles.sources = ../*.txt
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEPLOYMENT_PLUGIN += qcncodecs qjpcodecs qkrcodecs qtwcodecs
}



