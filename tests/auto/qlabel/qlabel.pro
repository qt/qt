load(qttest_p4)
SOURCES  += tst_qlabel.cpp

wince*:{
    DEFINES += SRCDIR=\\\"\\\"
    addFiles.sources = *.png testdata
    addFiles.path = .
    DEPLOYMENT += addFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}





