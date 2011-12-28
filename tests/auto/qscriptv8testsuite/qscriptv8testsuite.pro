load(qttest_p4)
QT = core script
SOURCES  += tst_qscriptv8testsuite.cpp
RESOURCES += qscriptv8testsuite.qrc
include(abstracttestsuite.pri)

CONFIG+=insignificant_test # QTQAINFRA-428
