#-------------------------------------------------
#
# Project created by QtCreator 2010-10-22T14:28:56
#
#-------------------------------------------------

QT       += core gui

TARGET = guitartuner
TEMPLATE = app

ICON = images/guitartuner_icon.svg

SOURCES += src/main.cpp\
    src/guitartuner.cpp \
    src/voicegenerator.cpp \
    src/voiceanalyzer.cpp \
    src/guitartunerui.cpp \
    src/fastfouriertransformer.cpp

HEADERS  += src/guitartuner.h \
    src/voicegenerator.h \
    src/voiceanalyzer.h \
    src/constants.h \
    src/guitartunerui.h \
    src/fastfouriertransformer.h

FORMS    += src/guitartunerui.ui

VERSION = 1.2.0

symbian {
    INCLUDEPATH += /epoc32/include/mmf/common
    INCLUDEPATH += /epoc32/include/mmf/server
    LIBS += -lmmfdevsound
    TARGET.UID3 = 0xEC46D2C8
    # TARGET.CAPABILITY +=
    # TARGET.EPOCSTACKSIZE = 0x14000
    # TARGET.EPOCHEAPSIZE = 0x020000 0x1000000
}

RESOURCES += \
    guitartuner.qrc

# Avoid auto screen rotation
DEFINES += ORIENTATIONLOCK

symbian {
    contains(DEFINES, ORIENTATIONLOCK):LIBS += -lavkon -leikcore -leiksrv -lcone
}


# Add mobility into Qt's CONFIG, and add  multimedia into MOBILITY.

win32 {
    QT += multimedia
    RC_FILE = src/guitartuner.rc
}
else {
    CONFIG += mobility
    MOBILITY += multimedia 
}

symbian {
    TARGET.CAPABILITY = UserEnvironment
}

# Add declarative module
QT       += declarative

OTHER_FILES += \
    src/application.qml \
    src/mycomponents/Adjuster.qml \
    src/mycomponents/adjustbars.js \
    src/mycomponents/ToggleButton.qml

OTHER_FILES += \
    src/mycomponents/NotesModel.qml \
    src/mycomponents/NoteButtonView.qml \
    src/mycomponents/Meter.qml

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}
