load(qttest_p4)
QT       += opengl declarative

TARGET = tst_nodestest
macx:CONFIG   -= app_bundle

SOURCES += tst_nodestest.cpp

CONFIG+=parallel_test

