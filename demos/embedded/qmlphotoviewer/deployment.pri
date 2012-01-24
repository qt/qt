qmlphotoviewer_src = $$PWD/../../declarative/photoviewer
symbian {
    load(data_caging_paths)
    qmlphotoviewer_uid3 = A000E400
    qmlphotoviewer_files.path = $$APP_PRIVATE_DIR_BASE/$$qmlphotoviewer_uid3
}
qmlphotoviewer_files.files = \
    $$qmlphotoviewer_src/qml/photoviewer/photoviewer.qml \
    $$qmlphotoviewer_src/qml/photoviewer/PhotoViewerCore
DEPLOYMENT += qmlphotoviewer_files
