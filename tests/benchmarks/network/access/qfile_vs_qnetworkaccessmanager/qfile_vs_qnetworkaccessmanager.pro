load(qttest_p4)
TEMPLATE = app
TARGET = tst_qfile_vs_qnetworkaccessmanager
DEPENDPATH += .
INCLUDEPATH += .

QT -= gui
QT += network

CONFIG += release

# Input
SOURCES += main.cpp
