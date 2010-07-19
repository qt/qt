qmlclocks_src = $$PWD/../../../examples/declarative/toys/clocks
symbian {
    load(data_caging_paths)
    qmlclocks_uid3 = A000E3FC
    qmlclocks_files.path = $$APP_PRIVATE_DIR_BASE/$$qmlclocks_uid3
}
qmlclocks_files.sources = $$qmlclocks_src/clocks.qml $$qmlclocks_src/content
DEPLOYMENT += qmlclocks_files
