TEMPLATE = subdirs
contains(QT_CONFIG, openvg):contains(QT_CONFIG, egl) {
    SUBDIRS += openvglite
}

    SUBDIRS += minimal

#this don't work. but leave it for now
qpa:x11 {
    SUBDIRS += testlite
}
