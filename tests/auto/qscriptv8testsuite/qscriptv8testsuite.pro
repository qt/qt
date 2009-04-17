load(qttest_p4)
QT = core script
SOURCES  += tst_qscriptv8testsuite.cpp

wince*: {
testFiles.sources = tests
testFiles.path = .
DEPLOYMENT += testFiles
}


