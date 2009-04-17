load(qttest_p4)
SOURCES         += ../tst_qpluginloader.cpp
TARGET  = ../tst_qpluginloader
QT = core

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qpluginloader
} else {
    TARGET = ../../release/tst_qpluginloader
  }
}


wince*: {
   addFiles.sources = ../bin/*.dll
   addFiles.path = bin
   DEPLOYMENT += addFiles
}

