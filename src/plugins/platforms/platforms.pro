TEMPLATE = subdirs

SUBDIRS += minimal

contains(QT_CONFIG, wayland) {
    SUBDIRS += wayland
}

qnx-*-qcc {
    SUBDIRS += blackberry
}
