load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativeproperty.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"

CONFIG += parallel_test

