load(qttest_p4)
TEMPLATE = app
TARGET = tst_binding
QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_binding.cpp testtypes.cpp
HEADERS += testtypes.h

symbian* {
    data.sources = data/*
    data.path = data
    DEPLOYMENT = data
}

