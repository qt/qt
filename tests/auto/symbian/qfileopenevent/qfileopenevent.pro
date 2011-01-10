load(qttest_p4)
HEADERS +=
SOURCES += tst_qfileopenevent.cpp
symbian {
    LIBS+=-lefsrv
}
