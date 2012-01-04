CONFIG += testcase
TARGET = tst_qtimeline
QT = core testlib
SOURCES  += tst_qtimeline.cpp
CONFIG += parallel_test

CONFIG+=insignificant_test # QTQAINFRA-428
