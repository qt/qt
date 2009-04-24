load(qttest_p4)
contains(QT_CONFIG,dbus): {
	TEMPLATE = subdirs
	CONFIG += ordered
	SUBDIRS = qpong test
} else {
	SOURCES += dummy.cpp
}


