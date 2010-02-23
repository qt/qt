TEMPLATE = app
TARGET = player

QT += gui multimedia


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
INSTALLS += target

