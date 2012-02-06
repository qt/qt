load(qttest_p4)
SOURCES  += tst_qstring.cpp

symbian:LIBS += -llibm

QT = core

DEFINES += QT_NO_CAST_TO_ASCII
CONFIG += parallel_test

contains(QT_CONFIG,icu):DEFINES += QT_USE_ICU

contains(QT_CONFIG, qt3support): DEFINES += QT3_SUPPORT
