CONFIG += testcase
TARGET = tst_qabstractfileengine
QT = core testlib

SOURCES = tst_qabstractfileengine.cpp
RESOURCES += qabstractfileengine.qrc

CONFIG+=insignificant_test # QTQAINFRA-428
