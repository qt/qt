qmltwitter_src = $$PWD/../../declarative/twitter
symbian {
    qmltwitter_uid3 = A000E401
    qmltwitter_files.path = $$APP_PRIVATE_DIR_BASE/$$qmltwitter_uid3
}
qmltwitter_files.sources = $$qmltwitter_src/twitter.qml $$qmltwitter_src/TwitterCore
DEPLOYMENT += qmltwitter_files
