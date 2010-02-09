load(qttest_p4)

SOURCES += tst_maketestselftest.cpp
QT = core

DEFINES += SRCDIR=\\\"$$PWD/\\\"

requires(!cross_compile)

