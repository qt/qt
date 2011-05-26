load(qttest_p4)
TEMPLATE = app
TARGET = tst_bench_qpainter

SOURCES += tst_qpainter.cpp

symbian: TARGET.EPOCHEAPSIZE = 0x100000 0x3000000
