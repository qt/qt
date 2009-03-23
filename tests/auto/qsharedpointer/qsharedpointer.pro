load(qttest_p4)

SOURCES += tst_qsharedpointer.cpp
QT = core
DEFINES += SRCDIR=\\\"$$PWD\\\"

include(externaltests.pri)
