load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative network
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativeengine.cpp 

!symbian: {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test

