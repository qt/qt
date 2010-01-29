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

win32 {
    !wince* {
        LIBS += -lWs2_32
    } else {
        LIBS += -lWs2
    }
}

CONFIG += console

symbian:TARGET.CAPABILITY = NetworkServices ReadUserData
