load(qttest_p4)
SOURCES += tst_qlabel.cpp
wince*::DEFINES += SRCDIR=\\\"\\\"
else:!symbian:DEFINES += SRCDIR=\\\"$$PWD/\\\"
wince*|symbian { 
    addFiles.sources = *.png \
        testdata
    addFiles.path = .
    DEPLOYMENT += addFiles
}
