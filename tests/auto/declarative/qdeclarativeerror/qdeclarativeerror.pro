load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_qdeclarativeerror.cpp
macx:CONFIG -= app_bundle

!symbian: {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test

CONFIG+=insignificant_test # QTQAINFRA-428
