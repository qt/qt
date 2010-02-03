TEMPLATE = subdirs

!maemo:contains(QT_CONFIG, dbus):contains(QT_CONFIG, networkmanager):SUBDIRS += networkmanager
win32:SUBDIRS += nla
win32:!wince*:SUBDIRS += nativewifi
macx:SUBDIRS += corewlan
symbian:SUBDIRS += symbian

isEmpty(SUBDIRS):SUBDIRS += generic
