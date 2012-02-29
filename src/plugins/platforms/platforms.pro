TEMPLATE = subdirs

SUBDIRS += minimal

contains(QT_CONFIG, wayland) {
    SUBDIRS += wayland
}

blackberry-armv7le-qcc {
    SUBDIRS += blackberry
}
