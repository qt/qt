load(qttest_p4)
TEMPLATE = app
TARGET = tst_qtemporaryfile
DEPENDPATH += .
INCLUDEPATH += .

QT -= gui

CONFIG += release

# Input
SOURCES += main.cpp
