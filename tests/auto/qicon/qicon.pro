load(qttest_p4)

SOURCES += tst_qicon.cpp
RESOURCES = tst_qicon.qrc

wince* {
   QT += xml svg
   addFiles.sources += $$_PRO_FILE_PWD_/*.png
   addFiles.sources += $$_PRO_FILE_PWD_/*.tga
   addFiles.sources += $$_PRO_FILE_PWD_/*.svg
   addFiles.sources += $$_PRO_FILE_PWD_/*.svgz
   addFiles.path = .
   DEPLOYMENT += addFiles

   DEPLOYMENT_PLUGIN += qsvg
   DEFINES += SRCDIR=\\\".\\\"
} else:symbian {
   QT += xml svg
   addFiles.sources =  *.png *.tga *.svg *.svgz
   addFiles.path = .
   plugins.sources = qsvgicon.dll
   plugins.path = iconengines
   DEPLOYMENT += addFiles plugins
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}

