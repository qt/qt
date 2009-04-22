TEMPLATE = app
TARGET = 
QT = core
win32: CONFIG += console
mac:CONFIG -= app_bundle
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
