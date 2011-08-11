#! [checking for a specific version of Qt]
contains(QT_VERSION, "4.7.4") {
    message(4.7.4)
} else {
    message(Some other version)
}
#! [checking for a specific version of Qt]
