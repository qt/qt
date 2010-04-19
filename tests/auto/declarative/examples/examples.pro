load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_examples.cpp 

include(../../../../tools/qml/qml.pri)

DEFINES += SRCDIR=\\\"$$PWD\\\"

CONFIG += parallel_test

