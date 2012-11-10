CONFIG += testcase

TARGET = tst_exceptionsafety_objects
QT += testlib
SOURCES += tst_exceptionsafety_objects.cpp
HEADERS += oomsimulator.h 3rdparty/valgrind.h 3rdparty/memcheck.h

CONFIG+=insignificant_test # QTBUG-18927
