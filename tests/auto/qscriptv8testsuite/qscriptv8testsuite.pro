load(qttest_p4)
QT = core script
SOURCES  += tst_qscriptv8testsuite.cpp
!symbian:DEFINES += SRCDIR=\\\"$$PWD\\\"

wince*|symbian: {
testFiles.files = tests
testFiles.path = .
DEPLOYMENT += testFiles
}
