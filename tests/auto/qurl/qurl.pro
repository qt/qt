CONFIG += testcase
TARGET = tst_qurl
QT = core testlib
SOURCES = tst_qurl.cpp
symbian: TARGET.CAPABILITY = NetworkServices
CONFIG += parallel_test
