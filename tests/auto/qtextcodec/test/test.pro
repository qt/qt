load(qttest_p4)
TARGET = ../tst_qtextcodec
SOURCES  += ../tst_qtextcodec.cpp
wince*|symbian {
   addFiles.sources = ../*.txt
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEPLOYMENT_PLUGIN += qcncodecs qjpcodecs qkrcodecs qtwcodecs
   DEFINES += SRCDIR=\\\"\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD/../\\\"
}
