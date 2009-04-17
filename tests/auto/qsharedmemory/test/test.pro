load(qttest_p4)

include(../src/src.pri)
QT -= gui

DEFINES	+= QSHAREDMEMORY_DEBUG
DEFINES	+= QSYSTEMSEMAPHORE_DEBUG

SOURCES += ../tst_qsharedmemory.cpp
TARGET = ../tst_qsharedmemory

!wince*:win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qsharedmemory
} else {
    TARGET = ../../release/tst_qsharedmemory
  }
}

wince*: {
QT += gui script
addFiles.sources = ../lackey/lackey.exe ../lackey/scripts
addFiles.path = lackey
DEPLOYMENT += addFiles
DEFINES += SRCDIR=\\\"\\\"
} else {
DEFINES += SRCDIR=\\\"$$PWD/../\\\"
}

