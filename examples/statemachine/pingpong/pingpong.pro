QT = core
TEMPLATE = app
TARGET = 
win32: CONFIG += console
mac:CONFIG -= app_bundle
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
