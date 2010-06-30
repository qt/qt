qmlflickr_src = $$PWD/../../declarative/flickr
symbian {
    qmlflickr_uid3 = A000E3FF
    qmlflickr_files.path = $$APP_PRIVATE_DIR_BASE/$$qmlflickr_uid3
}
qmlflickr_files.sources = $$qmlflickr_src/flickr.qml $$qmlflickr_src/common $$qmlflickr_src/mobile
DEPLOYMENT += qmlflickr_files
