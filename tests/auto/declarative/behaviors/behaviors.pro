load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_behaviors.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"
