TEMPLATE = app
TARGET =
INCLUDEPATH += .

HEADERS += desktopwidget.h contenttab.h linktab.h
SOURCES += desktopwidget.cpp contenttab.cpp linktab.cpp main.cpp

RESOURCES += desktopservices.qrc

music.sources = data/*.mp3 data/*.wav
music.path = /data/sounds/

image.sources = data/*.png
image.path = /data/images/

DEPLOYMENT += music image

include($$QT_SOURCE_TREE/demos/demobase.pri)

symbian {
    TARGET.UID3 = 0xA000C611
    ICON = ./resources/heart.svg
}
