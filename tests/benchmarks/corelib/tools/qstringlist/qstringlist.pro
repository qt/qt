load(qttest_p4)
TARGET = tst_bench_qstringlist
QT -= gui
SOURCES += main.cpp

symbian: LIBS += -llibpthread
