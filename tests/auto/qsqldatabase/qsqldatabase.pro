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

symbian {
	TARGET.EPOCHEAPSIZE=5000 5000000
	TARGET.EPOCSTACKSIZE=50000

    contains(S60_VERSION, 3.1)|contains(S60_VERSION, 3.2)|contains(S60_VERSION, 5.0) {
        sqlite.path = /sys/bin
        sqlite.sources = sqlite3.dll
        DEPLOYMENT += sqlite
    }
}

