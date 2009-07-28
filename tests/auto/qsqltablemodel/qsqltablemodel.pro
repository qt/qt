load(qttest_p4)
SOURCES  += tst_qsqltablemodel.cpp

QT += sql

wince*: {
   plugFiles.sources = ../../../plugins/sqldrivers
   plugFiles.path    = .
   DEPLOYMENT += plugFiles
   LIBS += -lws2
} else {
   win32:LIBS += -lws2_32
}

