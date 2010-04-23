load(qttest_p4)
TEMPLATE = app
TARGET = tst_qdeclarativecomponent
QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativecomponent.cpp testtypes.cpp
HEADERS += testtypes.h

# Define SRCDIR equal to test's source directory
DEFINES += SRCDIR=\\\"$$PWD\\\"

symbian* {
    data.sources = data/*
    data.path = data
    samegame.sources = data/samegame/*
    samegame.path = data/samegame
    samegame_pics.sources = data/samegame/pics/*
    samegame_pics.path = data/samegame/pics
    DEPLOYMENT += data samegame samegame_pics
}

