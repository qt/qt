load(qttest_p4)
TEMPLATE = app
TARGET = tst_bench_qscriptclass_bytearray

SOURCES += tst_qscriptclass_bytearray.cpp
RESOURCES += qscriptclass_bytearray.qrc

include($$QT_SOURCE_TREE/examples/script/customclass/bytearrayclass.pri)

QT = core script
