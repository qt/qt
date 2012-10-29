CONFIG += testcase

TARGET = ../tst_qsharedmemory
QT = core testlib
SOURCES += ../tst_qsharedmemory.cpp

include(../src/src.pri)

DEFINES	+= QSHAREDMEMORY_DEBUG
DEFINES	+= QSYSTEMSEMAPHORE_DEBUG

!wince*:win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qsharedmemory
  } else {
    TARGET = ../../release/tst_qsharedmemory
  }
}

wince*:{
requires(contains(QT_CONFIG,script))
QT += gui script
addFiles.files = $$OUT_PWD/../../lackey/lackey.exe ../../lackey/scripts
addFiles.path = .
DEPLOYMENT += addFiles
DEFINES += SRCDIR=\\\".\\\"
}else:symbian{
requires(contains(QT_CONFIG,script))
QT += gui script
addFiles.files = ../../lackey/scripts
addFiles.path = /data/qsharedmemorytemp/lackey
addBin.files = lackey.exe
addBin.path = /sys/bin
DEPLOYMENT += addFiles addBin
} else {
DEFINES += SRCDIR=\\\"$$PWD/../\\\"
}

CONFIG+=insignificant_test # QTQAINFRA-574
