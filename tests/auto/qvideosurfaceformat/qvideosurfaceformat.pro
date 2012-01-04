CONFIG += testcase

TARGET = tst_qvideosurfaceformat
QT += multimedia testlib
SOURCES += tst_qvideosurfaceformat.cpp

requires(contains(QT_CONFIG, multimedia))
