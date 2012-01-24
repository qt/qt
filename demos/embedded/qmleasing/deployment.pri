qmleasing_src = $$PWD/../../../examples/declarative/animation/easing
symbian {
    load(data_caging_paths)
    qmleasing_uid3 = A000E3FE
    qmleasing_files.path = $$APP_PRIVATE_DIR_BASE/$$qmleasing_uid3
}
qmleasing_files.files = \
    $$qmleasing_src/qml/easing/easing.qml \
    $$qmleasing_src/qml/easing/content
DEPLOYMENT += qmleasing_files
