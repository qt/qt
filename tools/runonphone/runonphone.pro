TEMPLATE = app

QT -= gui
CONFIG += console
CONFIG -= app_bundle

include(trk/trk.pri)

SOURCES += main.cpp \
    trksignalhandler.cpp

HEADERS += trksignalhandler.h \
    serenum.h

windows { 
    SOURCES += serenum_win.cpp 
    LIBS += -lsetupapi \
        -luuid \
        -ladvapi32
}
unix:!symbian {
    SOURCES += serenum_unix.cpp
}
else {
    SOURCES += serenum_stub.cpp
}
