qmleasing_src = $$PWD/../../../examples/declarative/animation/easing
symbian {
    qmleasing_uid3 = E8E8E725
    qmleasing_files.path = ../$$qmleasing_uid3
}
qmleasing_files.sources = $$qmleasing_src/easing.qml
DEPLOYMENT += qmleasing_files
