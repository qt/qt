load(qttest_p4)
SOURCES  += tst_qsqlquerymodel.cpp

QT += sql

wince*: {
   DEPLOYMENT_PLUGIN += qsqlite
} else {
   win32:LIBS += -lws2_32
}

