HEADERS += $$PWD/qsql_tds.h
SOURCES += $$PWD/qsql_tds.cpp

unix {
    !contains(LIBS, .*sybdb.*):LIBS += -lsybdb
} else:win32-borland {
    LIBS *= $(BCB)/lib/PSDK/NTWDBLIB.LIB
} else {
    LIBS *= -lNTWDBLIB
}
