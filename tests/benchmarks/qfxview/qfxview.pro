load(qttest_p4)
TEMPLATE = app
TARGET = tst_qfxview
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += release

QT += declarative script

DEFINES+=SRCDIR=\"$$PWD\"

# Input
SOURCES +=          \
    main.cpp

