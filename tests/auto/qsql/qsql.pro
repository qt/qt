load(qttest_p4)
SOURCES  += tst_qsql.cpp

QT += sql 

contains(QT_CONFIG, qt3support): QT += qt3support

wince*: {
   DEPLOYMENT_PLUGIN += qsqlite
}
