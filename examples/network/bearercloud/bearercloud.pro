HEADERS = bearercloud.h \
          cloud.h

SOURCES = main.cpp \
          bearercloud.cpp \
          cloud.cpp

RESOURCES = icons.qrc

TARGET = bearercloud

QT = core gui network svg

INCLUDEPATH += ../../src/bearer

include(../examples.pri)

CONFIG += mobility
MOBILITY = bearer

CONFIG += console

symbian:TARGET.CAPABILITY = NetworkServices ReadUserData
