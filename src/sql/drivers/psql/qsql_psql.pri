HEADERS += $$PWD/qsql_psql.h
SOURCES += $$PWD/qsql_psql.cpp

unix|win32-g++* {
    !static:!isEmpty(QT_LFLAGS_PSQL) {
        !contains(QT_CONFIG, system-zlib): QT_LFLAGS_PSQL -= -lz
        LIBS *= $$QT_LFLAGS_PSQL
        QMAKE_CXXFLAGS *= $$QT_CFLAGS_PSQL
    }
    !contains(LIBS, .*pq.*):LIBS += -lpq
} else {
    !contains(LIBS, .*pq.*):LIBS += -llibpq -lws2_32 -ladvapi32
}
