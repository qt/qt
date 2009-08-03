load(qttest_p4)
TARGET = ../tst_qtextcodec
SOURCES  += ../tst_qtextcodec.cpp
wince*|symbian {
   addFiles.sources = ../*.txt
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEPLOYMENT_PLUGIN += qcncodecs qjpcodecs qkrcodecs qtwcodecs
}

wince*: {
   DEFINES += SRCDIR=\\\"\\\"
}else:symbian {
    # Symbian can't define SRCDIR meaningfully here   
} else {
   DEFINES += SRCDIR=\\\"$$PWD/../\\\"
}
