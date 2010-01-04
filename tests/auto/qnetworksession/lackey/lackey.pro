SOURCES += main.cpp
TARGET = qnetworksessionlackey
INCLUDEPATH += ../../../../src/bearer
DEPENDPATH += ../../../../src/bearer

QT = core network

CONFIG+= testcase

include(../../../../common.pri)

CONFIG += mobility
MOBILITY = bearer
