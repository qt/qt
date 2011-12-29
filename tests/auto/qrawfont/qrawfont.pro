CONFIG += testcase

TARGET = tst_qrawfont
QT = core gui testlib
SOURCES += tst_qrawfont.cpp

INCLUDEPATH += $$QT_SOURCE_TREE/src/3rdparty/harfbuzz/src

wince*|symbian*: {
    DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
