load(qttest_p4)

SOURCES += tst_qicon.cpp
RESOURCES = tst_qicon.qrc

wince* {
   QT += xml svg
   addFiles.sources += $$_PRO_FILE_PWD_/*.png
   addFiles.sources += $$_PRO_FILE_PWD_/*.svg
   addFiles.sources += $$_PRO_FILE_PWD_/*.svgz
   addFiles.sources += $$_PRO_FILE_PWD_/tst_qicon.cpp
   addFiles.path = .
   DEPLOYMENT += addFiles

   DEPLOYMENT_PLUGIN += qsvg
   DEFINES += SRCDIR=\\\".\\\"
} else:symbian {
   QT += xml svg
   addFiles.sources =  *.png tst_qicon.cpp *.svg *.svgz
   addFiles.path = .
   DEPLOYMENT += addFiles
   qt_not_deployed {
      plugins.sources = qsvgicon.dll
      plugins.path = iconengines
      DEPLOYMENT += plugins
   }
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}

