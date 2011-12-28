CONFIG += testcase
TARGET = tst_qdbusservicewatcher
QT = core dbus testlib
contains(QT_CONFIG,dbus): {
        SOURCES += tst_qdbusservicewatcher.cpp
        QT += dbus
} else {
        SOURCES += ../qdbusmarshall/dummy.cpp
}
