load(qttest_p4)
TEMPLATE = app
win32:TARGET = ../clickLock
!win32:TARGET = clickLock

contains(QT_CONFIG, qt3support): QT += qt3support
DEPENDPATH += .
INCLUDEPATH += .
CONFIG -= app_bundle

# Input
SOURCES += main.cpp


