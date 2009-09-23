load(qttest_p4)

SOURCES += tst_qaudiooutput.cpp

QT = core multimedia

wince* {
    deploy.sources += 4.wav
    DEPLOYMENT = deploy
    DEFINES += SRCDIR=\\\"\\\"
    QT += gui
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
