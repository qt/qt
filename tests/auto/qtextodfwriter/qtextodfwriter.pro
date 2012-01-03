CONFIG += testcase

TARGET = tst_qtextodfwriter
QT += testlib
SOURCES += tst_qtextodfwriter.cpp

!symbian:DEFINES += SRCDIR=\\\"$$PWD\\\"
symbian:INCLUDEPATH+=$$[QT_INSTALL_PREFIX]/include/QtGui/private
