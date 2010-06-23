qmleasing_src = $$PWD/../../../examples/declarative/animation/easing
symbian {
    qmleasing_uid3 = A000E3FE
    qmleasing_files.path = ../$$qmleasing_uid3
}
qmleasing_files.sources = $$qmleasing_src/easing.qml
DEPLOYMENT += qmleasing_files
