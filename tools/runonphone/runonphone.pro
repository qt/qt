TEMPLATE = app

QT -= gui
CONFIG += console
CONFIG -= app_bundle

include(symbianutils/symbianutils.pri)

SOURCES += main.cpp \
    trksignalhandler.cpp

HEADERS += trksignalhandler.h \
    serenum.h

DEFINES += SYMBIANUTILS_INCLUDE_PRI

windows { 
    SOURCES += serenum_win.cpp 
    LIBS += -lsetupapi \
        -luuid \
        -ladvapi32
}
else:unix:!symbian {
    SOURCES += serenum_unix.cpp
    LIBS += -lusb
}
else {
    SOURCES += serenum_stub.cpp
}


