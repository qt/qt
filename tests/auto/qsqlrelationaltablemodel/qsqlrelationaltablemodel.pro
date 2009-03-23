load(qttest_p4)
SOURCES  += tst_qsqlrelationaltablemodel.cpp

QT += sql

wince*: {
   plugFiles.sources = ../../../plugins/sqldrivers
   plugFiles.path    = .
   DEPLOYMENT += plugFiles 
} else {
   win32-g++ {
        LIBS += -lws2_32
   } else:win32 {
        LIBS += ws2_32.lib
   }
}
