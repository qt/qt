CONFIG += testcase

TARGET = tst_qfileopenevent
QT += testlib
SOURCES += tst_qfileopenevent.cpp

HEADERS +=
symbian {
    LIBS+=-lefsrv -lapgrfx -lapmime
}
