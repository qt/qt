load(qttest_p4)
SOURCES  += tst_qsqldatabase.cpp

QT += sql

contains(QT_CONFIG, qt3support): QT += qt3support

win32:!wince*: LIBS += -lws2_32

wince*: {
   DEPLOYMENT_PLUGIN += qsqlite

   testData.sources = testdata
   testData.path = .

   DEPLOYMENT += testData
}



