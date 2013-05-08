CONFIG += testcase

TARGET = tst_qtableview
QT += testlib
SOURCES  += tst_qtableview.cpp

win32:CONFIG+=insignificant_test # QTQAINFRA-574
