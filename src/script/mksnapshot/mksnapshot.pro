TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

include(../v8.pri)
SOURCES += $$V8DIR/src/snapshot-empty.cc
SOURCES += $$V8DIR/src/mksnapshot.cc

LIBS += -L.. -lv8
