load(qttest_p4)
QT = core script
SOURCES  += tst_qscriptv8testsuite.cpp

wince*|symbian: {
testFiles.sources = tests
testFiles.path = .
DEPLOYMENT += testFiles
}

symbian:TARGET.EPOCHEAPSIZE = 0x00020000 0x02000000
