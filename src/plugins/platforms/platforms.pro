TEMPLATE = subdirs
contains(QT_CONFIG, openvg):contains(QT_CONFIG, egl) {
    embedded_lite {
        SUBDIRS += openvglite
    } 
}

embedded_lite {
    SUBDIRS += minimal
}

embedded_lite:x11 {
    SUBDIRS += testlite
}
