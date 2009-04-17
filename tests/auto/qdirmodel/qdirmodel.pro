load(qttest_p4)
SOURCES         += tst_qdirmodel.cpp

wince*: {
	addit.sources = dirtest\test1\*
	addit.path = dirtest\test1
	tests.sources = test\*
	tests.path = test
        sourceFile.sources = tst_qdirmodel.cpp
        sourceFile.path = .
	DEPLOYMENT += addit tests sourceFile
 DEFINES += SRCDIR=\\\"./\\\"
} else {
  DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

