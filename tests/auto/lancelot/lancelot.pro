load(qttest_p4)
QT += xml svg
contains(QT_CONFIG, opengl)|contains(QT_CONFIG, opengles1)|contains(QT_CONFIG, opengles2):QT += opengl

SOURCES += tst_lancelot.cpp

include($$QT_SOURCE_TREE/tests/arthur/common/common.pri)
include($$QT_SOURCE_TREE/tests/arthur/common/baselineprotocol.pri)

!symbian:!wince*:DEFINES += SRCDIR=\\\"$$PWD\\\"
