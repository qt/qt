load(qttest_p4)
QT = core script
SOURCES  += tst_qscriptjstestsuite.cpp
symbian {
	TARGET.EPOCHEAPSIZE = 0x020000 0xA00000
} else {
	DEFINES += SRCDIR=\\\"$$PWD\\\"
}
wince*|symbian*: {
testFiles.sources = tests
testFiles.path = .
DEPLOYMENT += testFiles
}


