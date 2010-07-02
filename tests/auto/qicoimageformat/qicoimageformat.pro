load(qttest_p4)
SOURCES+= tst_qicoimageformat.cpp

wince*: {
   DEFINES += SRCDIR=\\\".\\\"
   addFiles.sources = icons
   addFiles.path = .
   CONFIG(debug, debug|release):{
       addPlugins.sources = $$QT_BUILD_TREE/plugins/imageformats/qico4d.dll
   } else {
       addPlugins.sources = $$QT_BUILD_TREE/plugins/imageformats/qico4.dll
   }
   addPlugins.path = imageformats
   DEPLOYMENT += addFiles addPlugins
} else:symbian {
   addFiles.sources = icons
   addFiles.path = .
   DEPLOYMENT += addFiles
   qt_not_deployed {
      addPlugins.sources = qico.dll
      addPlugins.path = imageformats
      DEPLOYMENT += addPlugins
   }
   TARGET.UID3 = 0xE0340004
   DEFINES += SYMBIAN_SRCDIR_UID=$$lower($$replace(TARGET.UID3,"0x",""))
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}
