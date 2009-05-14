############################################################
# Project file for autotest for file q3datetimeedit.h
############################################################

load(qttest_p4)
QT += qt3support
requires(contains(QT_CONFIG,qt3support))

SOURCES += tst_q3datetimeedit.cpp


