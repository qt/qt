SOURCES  += networkmanagertest.cpp nmview.cpp
HEADERS  += nmview.h
TARGET = tst_networkmanagertest

QT = core network dbus gui

requires(contains(QT_CONFIG,dbus))

FORMS += dialog.ui
