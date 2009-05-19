load(qttest_p4)
SOURCES  += tst_q3sqlselectcursor.cpp

QT += sql qt3support
requires(contains(QT_CONFIG,qt3support))

win32:LIBS += -lws2_32



