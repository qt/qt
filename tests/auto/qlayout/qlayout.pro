############################################################
# Project file for autotest for file qlayout.h
############################################################

load(qttest_p4)

SOURCES += tst_qlayout.cpp
contains(QT_CONFIG, qt3support): QT += qt3support
wince*|symbian: {
   addFiles.sources = baseline
   addFiles.path = .
   DEPLOYMENT += addFiles
}

