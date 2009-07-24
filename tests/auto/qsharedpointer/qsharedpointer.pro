load(qttest_p4)
SOURCES += tst_qsharedpointer.cpp \
    forwarddeclaration.cpp \
    forwarddeclared.cpp
QT = core
!symbian:DEFINES += SRCDIR=\\\"$$PWD/\\\"
requires(contains(QT_CONFIG,private_tests))
include(externaltests.pri)
HEADERS += forwarddeclared.h
