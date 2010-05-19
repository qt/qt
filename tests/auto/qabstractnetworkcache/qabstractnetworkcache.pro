load(qttest_p4)
QT += network
QT -= gui
SOURCES  += tst_qabstractnetworkcache.cpp

wince*|symbian: {
   testFiles.sources = tests
   testFiles.path = .
   DEPLOYMENT += testFiles
}

symbian: TARGET.CAPABILITY = NetworkServices
