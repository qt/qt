load(qttest_p4)
SOURCES  += tst_qsqlfield.cpp

QT += sql

symbian {
    contains(S60_VERSION, 3.1)|contains(S60_VERSION, 3.2)|contains(S60_VERSION, 5.0) {
        sqlite.path = /sys/bin
        sqlite.sources = sqlite3.dll
        DEPLOYMENT += sqlite
    }
}

