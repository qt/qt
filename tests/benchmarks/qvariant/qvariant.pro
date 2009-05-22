load(qttest_p4)
TEMPLATE = app
TARGET = tst_qvariant
DEPENDPATH += .
INCLUDEPATH += .
QT -= gui

CONFIG += release
#CONFIG += debug


SOURCES += tst_qvariant.cpp
