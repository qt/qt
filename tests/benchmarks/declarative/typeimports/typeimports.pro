load(qttest_p4)
TEMPLATE = app
TARGET = tst_typeimports
QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_typeimports.cpp

symbian {
    data.sources = data
    data.path = .
    DEPLOYMENT += data
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}