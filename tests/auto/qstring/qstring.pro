load(qttest_p4)
SOURCES  += tst_qstring.cpp

symbian:LIBS += -llibm

QT = core

DEFINES += QT_NO_CAST_TO_ASCII
