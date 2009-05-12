load(qttest_p4)
TEMPLATE = app
win32:TARGET = ../clickLock
!win32:TARGET = clickLock

QT += qt3support
requires(contains(QT_CONFIG,qt3support))
DEPENDPATH += .
INCLUDEPATH += .
CONFIG -= app_bundle

# Input
SOURCES += main.cpp


