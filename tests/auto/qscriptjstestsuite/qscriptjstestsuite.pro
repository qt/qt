load(qttest_p4)
QT = core script
SOURCES  += tst_qscriptjstestsuite.cpp
DEFINES += SRCDIR=\\\"$$PWD\\\"

wince*: {
testFiles.sources = tests
testFiles.path = .
DEPLOYMENT += testFiles
}


