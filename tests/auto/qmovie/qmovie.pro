load(qttest_p4)
SOURCES += tst_qmovie.cpp
MOC_DIR=tmp

!contains(QT_CONFIG, no-gif):DEFINES += QTEST_HAVE_GIF
!contains(QT_CONFIG, no-jpeg):DEFINES += QTEST_HAVE_JPEG
!contains(QT_CONFIG, no-mng):DEFINES += QTEST_HAVE_MNG

wince*: {
   addFiles.sources = animations\\*
   addFiles.path = animations
   DEPLOYMENT += addFiles
}


symbian: {
   addFiles.sources = animations\\*
   addFiles.path = animations
   DEPLOYMENT += addFiles

   qt_not_deployed {
      imagePlugins.sources = qjpeg.dll qgif.dll qmng.dll
      imagePlugins.path = imageformats
      DEPLOYMENT += imagePlugins
   }
}
