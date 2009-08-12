load(qttest_p4)

SOURCES += tst_networkselftest.cpp
QT = core network
!symbian:DEFINES += SRCDIR=\\\"$$PWD\\\"

