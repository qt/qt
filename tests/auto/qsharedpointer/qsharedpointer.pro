load(qttest_p4)
SOURCES += tst_qsharedpointer.cpp \
    forwarddeclaration.cpp \
    forwarddeclared.cpp
QT = core
DEFINES += SRCDIR=\\\"$$PWD/\\\"
include(externaltests.pri)
HEADERS += forwarddeclared.h
