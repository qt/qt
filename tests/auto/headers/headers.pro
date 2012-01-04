CONFIG += testcase

TARGET = tst_headers
QT = core testlib
SOURCES  += tst_headers.cpp headersclean.cpp

CONFIG+=insignificant_test # QTQAINFRA-428
