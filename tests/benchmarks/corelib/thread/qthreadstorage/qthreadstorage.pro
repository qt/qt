load(qttest_p4)
TEMPLATE = app
TARGET = tst_bench_qthreadstorage

SOURCES += tst_qthreadstorage.cpp
QT -= gui

symbian: {
    TARGET.CAPABILITY = NetworkServices
    INCLUDEPATH *= $$MW_LAYER_SYSTEMINCLUDE
}
