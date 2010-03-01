load(qttest_p4)
TEMPLATE = app
TARGET = tst_qfileinfo
DEPENDPATH += .
INCLUDEPATH += .

QT -= gui

CONFIG += release

# Input
SOURCES += main.cpp
