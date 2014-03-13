load(qttest_p4)
SOURCES += ../tst_qdbusinterface.cpp
HEADERS += ../myobject.h
TARGET = ../tst_qdbusinterface

QT = core
QT += dbus

macx:CONFIG += insignificant_test # QTBUG-37469
