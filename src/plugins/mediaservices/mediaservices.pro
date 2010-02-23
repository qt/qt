TEMPLATE = subdirs

win32:!wince: SUBDIRS += directshow

mac: SUBDIRS += qt7

unix:!mac:!symbian:contains(QT_CONFIG, xvideo):contains(QT_CONFIG, gstreamer) {
    SUBDIRS += gstreamer
}
