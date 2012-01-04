CONFIG += testcase

TARGET = tst_qmdiarea
QT += testlib
SOURCES += tst_qmdiarea.cpp

INCLUDEPATH += .
DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII
contains(QT_CONFIG, opengl):QT += opengl

mac {
    LIBS += -framework Security
}

CONFIG+=insignificant_test # QTQAINFRA-428
