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
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}
