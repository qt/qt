include(../src/src.pri)

QT = core script network

CONFIG += qtestlib

DESTDIR = ./

win32: CONFIG += console
mac:CONFIG -= app_bundle

DEFINES += QLOCALSERVER_DEBUG
DEFINES += QLOCALSOCKET_DEBUG

SOURCES		+= main.cpp 
TARGET		= lackey


