load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
QT += script network
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativecomponent.cpp 

!symbian: {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test

