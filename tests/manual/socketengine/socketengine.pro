CONFIG += testcase
TEMPLATE = app
TARGET = tst_socketengine
DEPENDPATH += .
INCLUDEPATH += .

QT -= gui
QT += network testlib

CONFIG += release

symbian: TARGET.CAPABILITY = NetworkServices

# Input
SOURCES += main.cpp
