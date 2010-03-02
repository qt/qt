load(qttest_p4)
TEMPLATE = app
TARGET = tst_qdeclarativeimage
QT += declarative
macx:CONFIG -= app_bundle
CONFIG += release

SOURCES += tst_qdeclarativeimage.cpp

symbian* {
    data.sources = image.png
    data.path = .
    DEPLOYMENT += data
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}
