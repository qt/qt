load(qttest_p4)
SOURCES  += tst_qsocket.cpp

QT += network qt3support
requires(contains(QT_CONFIG,qt3support))


