CONFIG += testcase

TARGET = tst_qeventloop
QT = core network testlib
SOURCES += tst_qeventloop.cpp

win32:!wince*:LIBS += -luser32

symbian:TARGET.CAPABILITY += NetworkServices

CONFIG+=insignificant_test # QTQAINFRA-428
