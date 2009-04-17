load(qttest_p4)
SOURCES  += tst_qdir.cpp
RESOURCES      += qdir.qrc
wince*:{
  DirFiles.sources = testdir testdata searchdir resources unprintablenames entrylist types tst_qdir.cpp
  DirFiles.path = .
  DEPLOYMENT += DirFiles

  QT = core
  DEFINES += SRCDIR=\\\"\\\"
} else {
  QT = core
  contains(QT_CONFIG, qt3support):QT += qt3support
  DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

