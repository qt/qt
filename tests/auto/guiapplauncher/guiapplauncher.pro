# -------------------------------------------------
# Project created by QtCreator 2009-11-10T14:39:06
# -------------------------------------------------

# Link against gui for X11,etc.
load(qttest_p4)

DEFINES += SRCDIR=\\\"$$PWD/\\\"
TARGET = tst_guiapplauncher
CONFIG += console
CONFIG -= app_bundle
CONFIG += qtestlib
TEMPLATE = app
SOURCES += tst_guiapplauncher.cpp \
    windowmanager.cpp
HEADERS += windowmanager.h

# process enumeration,etc.
win32:LIBS+=-luser32
x11 {
    LIBS += $$QMAKE_LIBS_X11
}

CONFIG+=insignificant_test # QTBUG-27938
