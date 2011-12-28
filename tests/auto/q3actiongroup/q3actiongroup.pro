load(qttest_p4)
QT += qt3support
requires(contains(QT_CONFIG,qt3support))
SOURCES += tst_q3actiongroup.cpp 

CONFIG+=insignificant_test # QTQAINFRA-428
