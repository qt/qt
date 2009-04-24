load(qttest_p4)
SOURCES  += tst_qfontdatabase.cpp
!symbian:DEFINES += SRCDIR=\\\"$$PWD\\\"

wince*|symbian {
    additionalFiles.sources = FreeMono.ttf
    additionalFiles.path = .
    DEPLOYMENT += additionalFiles
}

