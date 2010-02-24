TEMPLATE = subdirs

contains(QT_CONFIG, mediaservice) {
    win32:!wince: SUBDIRS += directshow

    mac: SUBDIRS += qt7

    unix:!mac:!symbian:contains(QT_CONFIG, xvideo):contains(QT_CONFIG, gstreamer) {
        SUBDIRS += gstreamer
    }
}
