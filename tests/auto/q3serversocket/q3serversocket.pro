load(qttest_p4)
SOURCES  += tst_q3serversocket.cpp


QT += network qt3support
requires(contains(QT_CONFIG,qt3support))


