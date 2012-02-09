load(qttest_p4)

CONFIG += qtestlib
SOURCES += tst_uic3.cpp
TARGET = tst_uic3

# This test is not run on wince (I think)
DEFINES += SRCDIR=\\\"$$PWD\\\"

requires(contains(QT_CONFIG,qt3support))
