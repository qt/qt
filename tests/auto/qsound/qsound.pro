load(qttest_p4)
SOURCES += tst_qsound.cpp

wince*|symbian: {
   deploy.files += 4.wav
   DEPLOYMENT += deploy
   !symbian:DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
macx:CONFIG += insignificant_test # QTBUG-43507
