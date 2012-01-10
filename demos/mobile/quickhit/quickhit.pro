QT       += core gui declarative opengl

VERSION = 1.1.1

TARGET = quickhit
TEMPLATE = app

SOURCES += main.cpp \
        mainwindow.cpp \
    gameengine.cpp \
    InvSounds.cpp \
    myeventfilter.cpp \
    ga_src/GEAudioBuffer.cpp \
    ga_src/GEAudioOut.cpp \
    ga_src/GEInterfaces.cpp

HEADERS  += mainwindow.h \
    gameengine.h \
    myeventfilter.h \
    plugins/levelplugininterface.h \
    InvSounds.h \
    ga_src/GEAudioBuffer.h \
    ga_src/GEAudioOut.h \
    ga_src/GEInterfaces.h

RESOURCES += resources.qrc

# QtMultimedia from Qt
QT += multimedia

symbian {
    # in Symbian 1 we do not have OpenGL available
    contains(SYMBIAN_VERSION, 9.4) {
        DEFINES += QT_NO_OPENGL
        QT -= opengl
        warning("No OpenGL support")
    }

    CONFIG += mobility
    MOBILITY = systeminfo
    TARGET = QuickHit
    TARGET.UID3 = 0xe7ced714

    TARGET.CAPABILITY = NetworkServices \
        Location \
        ReadUserData \
        WriteUserData \
        LocalServices \
        UserEnvironment

    LIBS += -lcone -leikcore -lavkon -lhwrmvibraclient

    ICON = icon.svg

    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x100000 0x2000000
}

maemo5 {
    CONFIG += mobility
    MOBILITY = systeminfo

    BINDIR = /opt/usr/bin
    DATADIR = /usr/share
    DEFINES += DATADIR=\\\"$$DATADIR\\\" \
        PKGDATADIR=\\\"$$PKGDATADIR\\\"

    INSTALLS += target \
        desktop \
        icon64

    target.path = $$BINDIR
    desktop.path = $$DATADIR/applications/hildon
    desktop.files += quickhit.desktop

    icon64.path = $$DATADIR/icons/hicolor/64x64/apps
    icon64.files += quickhit.png
}

OTHER_FILES += \
    Game.qml \
    Game.js \
    MyShip.qml \
    Missile.qml \
    Menu.qml \
    MenuItem.qml \
    Button.qml \
    Message.qml
