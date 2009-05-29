load(qttest_p4)
SOURCES  += ../tst_qfile.cpp

wince*:{
QT = core gui
files.sources += ..\dosfile.txt ..\noendofline.txt ..\testfile.txt \
		  ..\testlog.txt ..\two.dots.file ..\tst_qfile.cpp \
		  ..\Makefile ..\forCopying.txt ..\forRenaming.txt
files.path = .
resour.sources += ..\resources
resour.path = .

DEPLOYMENT = files resour
DEFINES += SRCDIR=\\\"\\\"
} else {
QT = core network
DEFINES += SRCDIR=\\\"$$PWD/../\\\"
}

RESOURCES      += ../qfile.qrc ../rename-fallback.qrc ../copy-fallback.qrc

TARGET = ../tst_qfile

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qfile
} else {
    TARGET = ../../release/tst_qfile
  }
  LIBS+=-lole32 -luuid
}


