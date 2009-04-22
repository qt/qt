TEMPLATE = app
TARGET = contacts

MOC_DIR = .moc
OBJECTS_DIR = .obj

QT += declarative script sql

unix {
    sqlitedb.output  = ${QMAKE_FILE_BASE}.sqlite
    sqlitedb.commands = rm -f ${QMAKE_FILE_OUT}; if which sqlite3 ; then sqlite3 ${QMAKE_FILE_OUT} < ${QMAKE_FILE_NAME} ; fi
    sqlitedb.input = SQL_SOURCES
    sqlitedb.CONFIG += no_link target_predeps

    QMAKE_EXTRA_COMPILERS += sqlitedb
    QMAKE_EXTRA_TARGETS = sqlitedb
}

SQL_SOURCES += data/contacts.sql

SOURCES += main.cpp

