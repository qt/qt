load(qttest_p4)
TEMPLATE = app
TARGET = tst_qdeclarativeimage
QT += declarative
macx:CONFIG -= app_bundle
CONFIG += release

SOURCES += tst_qdeclarativeimage.cpp

symbian {
    importFiles.sources = image.png
    importFiles.path = 
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

