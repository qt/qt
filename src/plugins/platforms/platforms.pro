TEMPLATE = subdirs
contains(QT_CONFIG, openvg):contains(QT_CONFIG, egl) {
    SUBDIRS += openvglite
}

    SUBDIRS += minimal

contains(QT_CONFIG, mitshm) {
    SUBDIRS += testlite
}

linux {
    SUBDIRS += linuxfb
}

unix {
    SUBDIRS +=  vnc \
                qvfb
}
