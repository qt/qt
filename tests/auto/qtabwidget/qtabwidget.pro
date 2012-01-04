CONFIG += testcase

TARGET = tst_qtabwidget
QT += testlib
SOURCES += tst_qtabwidget.cpp

INCLUDEPATH += ../

HEADERS +=  
contains(QT_CONFIG, qt3support): QT += qt3support

win32:!wince*:LIBS += -luser32
