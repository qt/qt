TEMPLATE = app
TARGET = player

QT += gui mediaservices

HEADERS = \
    player.h \
    playercontrols.h \
    playlistmodel.h \
    videowidget.h

SOURCES = \
    main.cpp \
    player.cpp \
    playercontrols.cpp \
    playlistmodel.cpp \
    videowidget.cpp

target.path = $$[QT_INSTALL_DEMOS]/multimedia/player
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_DEMOS]/multimedia/player

INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000E3FA
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
}

