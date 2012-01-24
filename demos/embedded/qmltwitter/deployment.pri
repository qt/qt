qmltwitter_src = $$PWD/../../declarative/twitter
symbian {
    load(data_caging_paths)
    qmltwitter_uid3 = A000E401
    qmltwitter_files.path = $$APP_PRIVATE_DIR_BASE/$$qmltwitter_uid3
}
qmltwitter_files.files = \
    $$qmltwitter_src/qml/twitter/twitter.qml \
    $$qmltwitter_src/qml/twitter/TwitterCore
DEPLOYMENT += qmltwitter_files
