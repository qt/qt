load(qttest_p4)
SOURCES += tst_qgraphicsanchorlayout1.cpp
CONFIG += parallel_test

macx:CONFIG+=insignificant_test # QTQAINFRA-574
