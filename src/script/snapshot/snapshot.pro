TEMPLATE = lib
CONFIG += staticlib

CONFIG += building-libs

win32|mac:!macx-xcode:CONFIG += debug_and_release
macx:CONFIG(debug, debug|release) {
    TARGET = snapshot_debug
}

include($$PWD/../v8/v8.pri)

isEmpty(V8SNAPSHOT) {
   cross_compile {
       warning(Snapshot generation disabled when cross-compiling)
       V8SNAPSHOT = no
   } else {
       V8SNAPSHOT = yes
   }
}

contains(V8SNAPSHOT,yes) {
    macx:CONFIG(debug, debug|release) {
        v8_mksnapshot.commands = ../mksnapshot/mksnapshot_debug ${QMAKE_FILE_OUT} --logfile $$V8_GENERATED_SOURCES_DIR/snapshot.log --log-snapshot-positions
        v8_mksnapshot.output = $$V8_GENERATED_SOURCES_DIR/snapshot_debug.cpp
    } else {
        v8_mksnapshot.commands = ../mksnapshot/mksnapshot ${QMAKE_FILE_OUT}
        CONFIG(debug, debug|release) v8_mksnapshot.commands += --logfile $$V8_GENERATED_SOURCES_DIR/snapshot.log --log-snapshot-positions
        v8_mksnapshot.output = $$V8_GENERATED_SOURCES_DIR/snapshot.cpp
    }
    DUMMY_FILE = $$PWD/../api/qscriptengine.cpp
    v8_mksnapshot.input = DUMMY_FILE
    v8_mksnapshot.variable_out = SOURCES
    v8_mksnapshot.dependency_type = TYPE_C
    v8_mksnapshot.name = generating[v8] ${QMAKE_FILE_OUT}
    silent:v8_mksnapshot.commands = @echo generating[v8] ${QMAKE_FILE_OUT} && $$v8_mksnapshot.commands
    QMAKE_EXTRA_COMPILERS += v8_mksnapshot
} else {
    SOURCES += $$V8DIR/src/snapshot-empty.cc
}
