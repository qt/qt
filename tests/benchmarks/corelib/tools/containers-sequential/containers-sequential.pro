load(qttest_p4)
TEMPLATE = app
TARGET = tst_bench_containers-sequential
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
QT -= gui

symbian: TARGET.EPOCHEAPSIZE = 0x100000 0x3000000
