TEMPLATE = subdirs

contains(QT_CONFIG, media-backend) {
    win32:!wince*: SUBDIRS += directshow

    mac: SUBDIRS += qt7

    unix:!mac:!symbian:contains(QT_CONFIG, gstreamer) {
        SUBDIRS += gstreamer
    }

    symbian:contains(QT_CONFIG, audio-routing-available) {
        SUBDIRS += symbian
    }
}
