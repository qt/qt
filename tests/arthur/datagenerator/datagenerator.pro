# -*- Mode: makefile -*-
COMMON_FOLDER = $$PWD/../common
include(../arthurtester.pri)
CONFIG += debug console
TEMPLATE = app
TARGET = datagenerator
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../bin

QT += svg opengl xml qt3support

# Input
HEADERS += datagenerator.h \
	xmlgenerator.h
SOURCES += main.cpp datagenerator.cpp \
	xmlgenerator.cpp 

DEFINES += QT_USE_USING_NAMESPACE

