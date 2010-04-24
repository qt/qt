load(qttest_p4)

SOURCES += tst_qsoundeffect.cpp

QT = core multimedia mediaservices

wince* {
    deploy.sources += 4.wav
    DEPLOYMENT = deploy
    DEFINES += SRCDIR=\\\"\\\"
    QT += gui
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

unix:!mac {
    !contains(QT_CONFIG, pulseaudio) {
        DEFINES += QT_MULTIMEDIA_QMEDIAPLAYER
    }
}
