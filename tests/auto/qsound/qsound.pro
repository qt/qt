load(qttest_p4)
SOURCES += tst_qsound.cpp

wince*|symbian: {
   deploy.sources += 4.wav
   DEPLOYMENT = deploy
   !symbian:DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

