qmlphotoviewer_src = $$PWD/../../declarative/photoviewer
symbian {
    qmlphotoviewer_uid3 = A000E400
    qmlphotoviewer_files.path = $$APP_PRIVATE_DIR_BASE/$$qmlphotoviewer_uid3
}
qmlphotoviewer_files.sources = $$qmlphotoviewer_src/photoviewer.qml $$qmlphotoviewer_src/PhotoViewerCore
DEPLOYMENT += qmlphotoviewer_files
