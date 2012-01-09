TARGET = tst_bench_qsqlquery

QT = core sql testlib
SOURCES += main.cpp

win32: LIBS += -lWs2_32
