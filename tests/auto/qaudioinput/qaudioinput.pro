CONFIG += testcase

TARGET = tst_qaudioinput
QT = core multimedia testlib
SOURCES += tst_qaudioinput.cpp

wince* {
    deploy.files += 4.wav
    DEPLOYMENT += deploy
    DEFINES += SRCDIR=\\\"\\\"
    QT += gui
} else {
    !symbian:DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
