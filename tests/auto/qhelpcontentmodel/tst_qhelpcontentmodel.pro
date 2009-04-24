load(qttest_p4)
SOURCES += tst_qhelpcontentmodel.cpp

CONFIG += help

DEFINES += SRCDIR=\\\"$$PWD\\\"
DEFINES += QT_USE_USING_NAMESPACE
!contains(QT_BUILD_PARTS, tools): DEFINES += QT_NO_BUILD_TOOLS
