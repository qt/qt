CONFIG += testcase

TARGET = tst_uic
QT += qtestlib
SOURCES += tst_uic.cpp

# This test is not run on wince (I think)
DEFINES += SRCDIR=\\\"$$PWD/\\\"
