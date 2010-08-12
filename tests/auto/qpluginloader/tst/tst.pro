load(qttest_p4)
SOURCES         += ../tst_qpluginloader.cpp
TARGET  = ../tst_qpluginloader
QT = core
HEADERS += ../theplugin/plugininterface.h

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qpluginloader
} else {
    TARGET = ../../release/tst_qpluginloader
  }
}


wince*: {
   addFiles.sources = $$OUT_PWD/../bin/*.dll
   addFiles.path = bin
   DEPLOYMENT += addFiles
}

symbian: {
   libDep.sources = tst_qpluginloaderlib.dll
   libDep.path = /sys/bin
   pluginDep.sources = theplugin.dll
   pluginDep.path = bin

   DEPLOYMENT += libDep pluginDep
}
