load(qttest_p4)

SOURCES += tst_qaudiooutput.cpp

QT = core multimedia

wince*|symbian: {
    deploy.files += 4.wav
    DEPLOYMENT += deploy
    !symbian {
        DEFINES += SRCDIR=\\\"\\\"
        QT += gui
    }
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
