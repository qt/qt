CONFIG += testcase

TARGET = tst_qgraphicseffect
QT += testlib
SOURCES  += tst_qgraphicseffect.cpp
CONFIG += parallel_test

CONFIG+=insignificant_test # QTQAINFRA-428
