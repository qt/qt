load(qttest_p4)
SOURCES  += tst_qfontdatabase.cpp
DEFINES += SRCDIR=\\\"$$PWD\\\"

wince* {
    additionalFiles.sources = FreeMono.ttf
    additionalFiles.path = .
    DEPLOYMENT += additionalFiles
}

