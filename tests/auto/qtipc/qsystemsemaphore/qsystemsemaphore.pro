CONFIG += testcase

TARGET = tst_qsystemsemaphore
QT += testlib
SOURCES += tst_qsystemsemaphore.cpp

include(../qsharedmemory/src/src.pri)
win32: CONFIG += console

DEFINES	+= QSHAREDMEMORY_DEBUG
DEFINES	+= QSYSTEMSEMAPHORE_DEBUG

RESOURCES += files.qrc

wince*: {
requires(contains(QT_CONFIG,script))
# this test calls lackey, which then again depends on QtScript.
# let's add it here so that it gets deployed easily
QT += script
lackey.files = $$OUT_PWD/../lackey/lackey.exe ../lackey/scripts
lackey.path = .
DEPLOYMENT += lackey
}

symbian: {
requires(contains(QT_CONFIG,script))
# this test calls lackey, which then again depends on QtScript.
# let's add it here so that it gets deployed easily
QT += script

lackey.files = ../lackey/lackey.exe
lackey.path = /sys/bin
DEPLOYMENT += lackey

# PowerMgmt capability needed to kill lackey process
TARGET.CAPABILITY = PowerMgmt
}

