# -*- Mode: makefile -*-
COMMON_FOLDER = $$PWD/../common
include(../arthurtester.pri)
TEMPLATE = app
TARGET = shower
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../bin

QT += xml opengl svg qt3support

# Input
HEADERS += shower.h
SOURCES += main.cpp shower.cpp


