load(qttest_p4)
SOURCES  += tst_qsqlthread.cpp

QT = core sql


wince*: {
   plugFiles.sources = ../../../plugins/sqldrivers
   plugFiles.path    = .
   DEPLOYMENT += plugFiles 
} else {
   win32:LIBS += -lws2_32
}

