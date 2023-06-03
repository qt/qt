load(qttest_p4)
TEMPLATE = app
TARGET = jpeg
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += release

wince*: {
   DEFINES += SRCDIR=\\\"\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

# Input
SOURCES += jpeg.cpp
