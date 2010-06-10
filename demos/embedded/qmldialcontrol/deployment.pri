qmldialcontrol_src = $$PWD/../../../examples/declarative/ui-components/dialcontrol
symbian {
    qmldialcontrol_uid3 = E59A9283
    qmldialcontrol_files.path = ../$$qmldialcontrol_uid3
}
qmldialcontrol_files.sources = $$qmldialcontrol_src/dialcontrol.qml $$qmldialcontrol_src/content
DEPLOYMENT += qmldialcontrol_files
