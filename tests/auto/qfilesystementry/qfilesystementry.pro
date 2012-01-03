CONFIG += testcase

TARGET = tst_qfilesystementry
QT = core testlib
SOURCES   += tst_qfilesystementry.cpp \
    ../../../src/corelib/io/qfilesystementry.cpp
HEADERS += ../../../src/corelib/io/qfilesystementry_p.h

CONFIG += parallel_test
