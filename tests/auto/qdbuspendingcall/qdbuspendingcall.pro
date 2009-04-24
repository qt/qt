load(qttest_p4)
QT = core
contains(QT_CONFIG, dbus):QT += dbus
SOURCES += tst_qdbuspendingcall.cpp
