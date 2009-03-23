load(qttest_p4)
TEMPLATE = app
TARGET = tst_qdiriterator
DEPENDPATH += .
INCLUDEPATH += .

QT -= gui

CONFIG += release
CONFIG += debug


SOURCES += main.cpp

SOURCES += qfilesystemiterator.cpp
HEADERS += qfilesystemiterator.h

wince*: {
   corelibdir.sources = $$QT_SOURCE_TREE/src/corelib
   corelibdir.path = ./depot/src
   DEPLOYMENT += corelibdir
}

