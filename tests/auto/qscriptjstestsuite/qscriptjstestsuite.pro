load(qttest_p4)
QT = core script
SOURCES  += tst_qscriptjstestsuite.cpp

wince*: {
testFiles.sources = tests
testFiles.path = .
DEPLOYMENT += testFiles
}


