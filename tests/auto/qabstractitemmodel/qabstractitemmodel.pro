CONFIG += testcase

TARGET = tst_qabstractitemmodel
QT += testlib
SOURCES += tst_qabstractitemmodel.cpp ../modeltest/dynamictreemodel.cpp ../modeltest/modeltest.cpp

INCLUDEPATH += $$PWD/../modeltest
HEADERS     += ../modeltest/dynamictreemodel.h ../modeltest/modeltest.h
