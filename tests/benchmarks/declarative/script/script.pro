load(qttest_p4)
TEMPLATE = app
TARGET = tst_script
QT += declarative script
macx:CONFIG -= app_bundle
CONFIG += release

SOURCES += tst_script.cpp

symbian* {
    data.sources = data/*
    data.path = data
    DEPLOYMENT += data
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}




