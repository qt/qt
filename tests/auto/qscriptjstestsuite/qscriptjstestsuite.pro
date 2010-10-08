load(qttest_p4)
QT = core script
SOURCES  += tst_qscriptjstestsuite.cpp

!symbian: DEFINES += SRCDIR=\\\"$$PWD\\\"

wince*|symbian: {
testFiles.files = tests
testFiles.path = .
DEPLOYMENT += testFiles
}


