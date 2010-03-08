TEMPLATE = subdirs

!maemo:contains(QT_CONFIG, dbus):contains(QT_CONFIG, networkmanager) {
    SUBDIRS += networkmanager generic
}
#win32:SUBDIRS += nla
win32:SUBDIRS += generic
win32:!wince*:SUBDIRS += nativewifi
macx:contains(QT_CONFIG, corewlan):SUBDIRS += corewlan
macx:SUBDIRS += generic
symbian:SUBDIRS += symbian
maemo6:contains(QT_CONFIG, dbus):SUBDIRS += icd

isEmpty(SUBDIRS):SUBDIRS += generic
