QT       += core gui declarative
CONFIG   += mobility
MOBILITY += sensors

TARGET = qtbubblelevel
TEMPLATE = app

VERSION = 1.2.0

HEADERS += taskswitcher.h \
           accelerometerfilter.h \
           settings.h

SOURCES += main.cpp \
           taskswitcher.cpp \
           accelerometerfilter.cpp

OTHER_FILES += qml/*.qml \
               qml/images/*.png

RESOURCES = resources.qrc

maemo5 {
    BINDIR = /opt/usr/bin
    DATADIR = /usr/share
    DEFINES += DATADIR=\\\"$$DATADIR\\\" \
        PKGDATADIR=\\\"$$PKGDATADIR\\\"
    INSTALLS += target \
        desktop \
        iconxpm \
        icon26 \
        icon40 \
        icon64

    target.path = $$BINDIR
    desktop.path = $$DATADIR/applications/hildon
    desktop.files += $${TARGET}.desktop

    iconxpm.path = $$DATADIR/pixmap
    iconxpm.files += icons/xpm/qtbubblelevel.xpm

    icon26.path = $$DATADIR/icons/hicolor/26x26/apps
    icon26.files += icons/26x26/qtbubblelevel.png

    icon40.path = $$DATADIR/icons/hicolor/40x40/apps
    icon40.files += icons/40x40/qtbubblelevel.png

    icon64.path = $$DATADIR/icons/hicolor/64x64/apps
    icon64.files += icons/64x64/qtbubblelevel.png
}

symbian {
    TARGET = QtBubbleLevel

    # To lock the application to landscape orientation
    LIBS += -lcone -leikcore -lavkon

    ICON = icons/bubblelevel.svg
}
