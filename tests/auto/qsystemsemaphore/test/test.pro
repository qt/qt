CONFIG += qttest_p4
#QT = core

include(../../qsharedmemory/src/src.pri)
win32: CONFIG += console

DEFINES	+= QSHAREDMEMORY_DEBUG
DEFINES	+= QSYSTEMSEMAPHORE_DEBUG

SOURCES		+= ../tst_qsystemsemaphore.cpp 
TARGET		= tst_qsystemsemaphore
DESTDIR		= ../
win32 {
    CONFIG(debug, debug|release): DESTDIR = ../debug
    else: DESTDIR = ../release
}

RESOURCES += ../files.qrc

wince*: {
# this test calls lackkey, which then again depends on QtScript.
# let's add it here so that it gets deployed easily
QT += script
lackey.sources = ../../qsharedmemory/lackey/lackey.exe
lackey.path = ../qsharedmemory/lackey

DEPLOYMENT += lackey
}

