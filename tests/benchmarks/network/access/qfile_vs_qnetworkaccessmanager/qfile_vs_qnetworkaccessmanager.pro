load(qttest_p4)
TEMPLATE = app
TARGET = qfile_vs_qnetworkaccessmanager
DEPENDPATH += .
INCLUDEPATH += .

QT -= gui
QT += network

CONFIG += release

# Input
SOURCES += main.cpp
