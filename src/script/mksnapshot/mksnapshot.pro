TEMPLATE = app
QT =
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

include($$PWD/../v8/v8.pri)
SOURCES += $$V8DIR/src/snapshot-empty.cc
SOURCES += $$V8DIR/src/mksnapshot.cc

LIBS += -L../v8/ -lv8
