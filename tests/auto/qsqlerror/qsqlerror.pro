CONFIG += testcase

TARGET = tst_qsqlerror
QT = core sql testlib
SOURCES += tst_qsqlerror.cpp

symbian {
    qt_not_deployed {
        contains(S60_VERSION, 3.1)|contains(S60_VERSION, 3.2)|contains(S60_VERSION, 5.0) {
            sqlite.path = /sys/bin
            sqlite.files = sqlite3.dll
            DEPLOYMENT += sqlite
        }
    }
}
