load(qttest_p4)
SOURCES += tst_qsound.cpp

wince*: {
   deploy.sources += 4.wav
   DEPLOYMENT = deploy
}