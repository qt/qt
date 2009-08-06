load(qttest_p4)

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += tst_qaudiooutput.cpp

QT = core multimedia

wince*: {
   deploy.sources += 4.wav
   DEPLOYMENT = deploy
}
