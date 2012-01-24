qmlflickr_src = $$PWD/../../declarative/flickr
symbian {
    load(data_caging_paths)
    qmlflickr_uid3 = A000E3FF
    qmlflickr_files.path = $$APP_PRIVATE_DIR_BASE/$$qmlflickr_uid3
}
qmlflickr_files.files = \
    $$qmlflickr_src/qml/flickr/flickr.qml \
    $$qmlflickr_src/qml/flickr/common \
    $$qmlflickr_src/qml/flickr/mobile
DEPLOYMENT += qmlflickr_files
