load(qttest_p4)
contains(QT_CONFIG,dbus): {
	TEMPLATE = subdirs
	CONFIG += ordered
	SUBDIRS = server test
} else {
	SOURCES += ../qdbusmarshall/dummy.cpp
}
