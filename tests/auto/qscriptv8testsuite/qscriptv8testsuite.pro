load(qttest_p4)
QT = core script
SOURCES  += tst_qscriptv8testsuite.cpp
DEFINES += SRCDIR=\\\"$$PWD\\\"

wince*: {
testFiles.sources = tests
testFiles.path = .
DEPLOYMENT += testFiles
}


