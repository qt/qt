TEMPLATE = app
macx:CONFIG -= app_bundle
macx:CONFIG += debug_and_release

QT =
DEPENDPATH += .
INCLUDEPATH += .

win32|mac:!macx-xcode:CONFIG += debug_and_release
macx:CONFIG(debug, debug|release) {
    TARGET = mksnapshot_debug
    LIBS += -L../v8/ -lv8_debug
} else {
    LIBS += -L../v8/ -lv8
}

include($$PWD/../v8/v8.pri)

SOURCES += $$V8DIR/src/snapshot-empty.cc
SOURCES += $$V8DIR/src/mksnapshot.cc
