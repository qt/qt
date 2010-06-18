qmltwitter_src = $$PWD/../../declarative/twitter
symbian {
    qmltwitter_uid3 = EEF6D468
    qmltwitter_files.path = ../$$qmltwitter_uid3
}
qmltwitter_files.sources = $$qmltwitter_src/twitter.qml $$qmltwitter_src/TwitterCore
DEPLOYMENT += qmltwitter_files
