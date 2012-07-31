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

linux-*:system(". /etc/lsb-release && [ $DISTRIB_CODENAME = lucid ]"):DEFINES+=UBUNTU_LUCID # QTBUG-26726
