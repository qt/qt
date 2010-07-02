qmldialcontrol_src = $$PWD/../../../examples/declarative/ui-components/dialcontrol
symbian {
    qmldialcontrol_uid3 = A000E3FD
    qmldialcontrol_files.path = $$APP_PRIVATE_DIR_BASE/$$qmldialcontrol_uid3
}
qmldialcontrol_files.sources = $$qmldialcontrol_src/dialcontrol.qml $$qmldialcontrol_src/content
DEPLOYMENT += qmldialcontrol_files
