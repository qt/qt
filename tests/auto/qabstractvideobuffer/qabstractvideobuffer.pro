CONFIG += testcase

TARGET = tst_qabstractvideobuffer
QT += multimedia testlib
SOURCES += tst_qabstractvideobuffer.cpp

requires(contains(QT_CONFIG, multimedia))
