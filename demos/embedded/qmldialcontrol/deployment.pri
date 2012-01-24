qmldialcontrol_src = $$PWD/../../../examples/declarative/ui-components/dialcontrol
symbian {
    load(data_caging_paths)
    qmldialcontrol_uid3 = A000E3FD
    qmldialcontrol_files.path = $$APP_PRIVATE_DIR_BASE/$$qmldialcontrol_uid3
}
qmldialcontrol_files.files = \
    $$qmldialcontrol_src/qml/dialcontrol/dialcontrol.qml \
    $$qmldialcontrol_src/qml/dialcontrol/content
DEPLOYMENT += qmldialcontrol_files
