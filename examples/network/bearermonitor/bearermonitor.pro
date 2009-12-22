HEADERS = sessionwidget.h \
          bearermonitor.h

SOURCES = main.cpp \
          bearermonitor.cpp \
          sessionwidget.cpp
          
FORMS = bearermonitor_240_320.ui \
        bearermonitor_640_480.ui \
        sessionwidget.ui

TARGET = bearermonitor

QT = core gui network

INCLUDEPATH += ../../src/bearer

include(../examples.pri)

qtAddLibrary(QtBearer)
win32:!wince*:LIBS += -lWs2_32
wince*:LIBS += -lWs2

CONFIG += console

include(../examples.pri)

macx: {
    contains(QT_CONFIG,qt_framework):LIBS += -framework QtBearer
    contains(CONFIG, debug) {
     }
}
