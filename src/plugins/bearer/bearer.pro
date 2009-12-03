TEMPLATE = subdirs

SUBDIRS += generic
contains(QT_CONFIG, dbus):contains(QT_CONFIG, networkmanager):SUBDIRS += networkmanager
win32:SUBDIRS += nla
