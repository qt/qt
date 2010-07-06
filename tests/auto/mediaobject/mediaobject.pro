############################################################
# Project file for autotest for file mediaobject.h
############################################################

load(qttest_p4)

contains(QT_CONFIG, phonon):QT += phonon
SOURCES += tst_mediaobject.cpp
HEADERS += qtesthelper.h
RESOURCES += mediaobject.qrc

wince*{
  DEPLOYMENT_PLUGIN += phonon_waveout
  DEFINES += tst_MediaObject=tst_MediaObject_waveout
}

symbian:{
   addFiles.sources = media/test.sdp
   addFiles.path = media
   DEPLOYMENT += addFiles
}

