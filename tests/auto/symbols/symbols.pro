CONFIG += testcase

TARGET = tst_symbols
QT = core testlib
SOURCES += tst_symbols.cpp

cross_compile: DEFINES += QT_CROSS_COMPILED
