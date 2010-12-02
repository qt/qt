#-------------------------------------------------
#
# Project created by QtCreator 2010-08-11T11:51:09
#
#-------------------------------------------------

QT       += core network

# gui needed for QImage
# QT       -= gui

TARGET = baselineserver
DESTDIR = ../bin
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../../common/baselineprotocol.pri)

SOURCES += main.cpp \
    baselineserver.cpp \
    htmlpage.cpp

HEADERS += \
    baselineserver.h \
    htmlpage.h

RESOURCES += \
    baselineserver.qrc
