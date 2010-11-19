include(../../qpluginbase.pri)

TARGET = qtactilefeedback$${QT_LIBINFIX}

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/s60/feedback

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

contains(S60_VERSION, 5.0)|contains(S60_VERSION, symbian3) {
    HEADERS += qtactileFeedback.h
    SOURCES += qtactileFeedback_s60.cpp

    LIBS += -ltouchfeedback
}

load(data_caging_paths)

TARGET.UID3=0x200315B4
