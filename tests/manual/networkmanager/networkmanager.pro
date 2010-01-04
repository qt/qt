SOURCES  += networkmanagertest.cpp nmview.cpp
HEADERS  += nmview.h
TARGET = tst_networkmanagertest

QT = core network dbus gui

#not really a test case but deployment happens same way
CONFIG += testcase

requires(contains(QT_CONFIG,dbus))

INCLUDEPATH += ../../src/bearer
include(../../common.pri)

CONFIG += mobility
MOBILITY = bearer

#MOC_DIR = .moc
#OBJECTS_DIR = .obj
FORMS += dialog.ui
