load(qttest_p4)
SOURCES  += tst_qtranslator.cpp


wince*: {
   addFiles.sources = hellotr_la.qm
   addFiles.path = .
   DEPLOYMENT += addFiles
}


