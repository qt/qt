load(qttest_p4)
QT += network
SOURCES  += tst_qabstractnetworkcache.cpp

wince*|symbian: {
   testFiles.sources = tests
   testFiles.path = .
   DEPLOYMENT += testFiles
}

