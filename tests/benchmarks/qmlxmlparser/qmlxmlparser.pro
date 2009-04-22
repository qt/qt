load(qttest_p4)
TEMPLATE = app
TARGET = tst_qmlxmlparser
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += release

QT += declarative xml

DEFINES+=SRCDIR=\"$$PWD\"

# Input
SOURCES += main.cpp
