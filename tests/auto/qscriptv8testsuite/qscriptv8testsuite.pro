CONFIG += testcase

TARGET = tst_qscriptv8testsuite
QT = core script testlib
SOURCES  += tst_qscriptv8testsuite.cpp
RESOURCES += qscriptv8testsuite.qrc
include(abstracttestsuite.pri)

CONFIG+=insignificant_test # QTQAINFRA-428
