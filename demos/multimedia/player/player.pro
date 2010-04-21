
QT += multimedia

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

sources.files = $$SOURCES $$HEADERS
sources.path = $$[QT_INSTALL_DEMOS]/multimedia/player player.pro

INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000E3FA
}

