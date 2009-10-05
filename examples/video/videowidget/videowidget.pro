TEMPLATE = app

QT += multimedia

HEADERS = \
    videoplayer.h \
    videowidget.h \
    videowidgetsurface.h

SOURCES = \
    main.cpp \
    videoplayer.cpp \
    videowidget.cpp \
    videowidgetsurface.cpp

symbian {
    TARGET.UID3 = 0xA000D7C3
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
