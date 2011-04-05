load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_qsgcanvas.cpp

macx:CONFIG -= app_bundle

CONFIG += parallel_test
