load(qttest_p4)
TEMPLATE = app
TARGET = tst_bench_qgraphicsscene

SOURCES += tst_qgraphicsscene.cpp

symbian: TARGET.EPOCHEAPSIZE = 0x100000 0x3000000
